"""
Define Request sender class

 Revised BSD License
 Copyright Semtech Corporation 2020. All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the Semtech corporation nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
"""

from lr1110evk.BaseTypes import (
    ScannedMacAddress,
    ScannedGnss,
    GroupingMultiFrameNotEnoughNavException,
)
from .FileReader import FileReader
from .RequestBase import RequestWifiGls, RequestGnssGls, RequestGnssMultiFrameGls
from .GeoLocServiceClientBase import GeoLocServiceBadResponseStatus


class RequestSenderException(Exception):
    pass


class MultipleScanTypesException(RequestSenderException):
    pass


class TooManyNavMessagesException(RequestSenderException):
    def __init__(self, nav_message_list):
        self.nav_message_list = nav_message_list

    def __str__(self):
        return "Too many nav message reported for one job: {}".format(
            ",".join(self.nav_message_list)
        )


class NoNavMessageException(RequestSenderException):
    def __str__(self):
        return "No NAV message available"


class SolverContactException(RequestSenderException):
    def __init__(self, reason):
        self.reason = reason

    def __str__(self):
        return "Exception when contacting solver. Reason: '{}'".format(self.reason)


class RequestSender:
    def __init__(self, configuration):
        self.configuration = configuration
        self.GEOLOC_SERVICE_MAPPER = {
            RequestWifiGls: self.configuration.wifi_server,
            RequestGnssGls: self.configuration.gnss_server,
            RequestGnssMultiFrameGls: self.configuration.gnss_multiframe_server,
        }

    def build_wifi_requests(self, mac_addresses):
        wifi_data = [
            data for data in mac_addresses if isinstance(data, ScannedMacAddress)
        ]
        request = RequestWifiGls()
        request.macs = wifi_data
        return request

    def build_gnss_requests_from_strategy(self):
        return self.configuration.gnss_solver_strategy.build_request()

    def build_gnss_multiframe_requests(self, multi_frame_container):
        nav_messages = [
            gnss_data.nav_message for gnss_data in multi_frame_container.get_navs()
        ]
        if not nav_messages:
            raise NoNavMessageException()
        request = RequestGnssMultiFrameGls(nav_messages=nav_messages)
        return request

    def get_geo_loc_service_for_request(self, request):
        return self.GEOLOC_SERVICE_MAPPER[request.__class__]

    def send_request_get_response(self, request):
        geoloc_service = self.get_geo_loc_service_for_request(request)
        try:
            response = geoloc_service.call_service_and_get_response(request.to_json())
        except GeoLocServiceBadResponseStatus as bad_response:
            raise SolverContactException(
                reason="{}: {}".format(
                    bad_response.bad_http_code_text, bad_response.erroneous_response
                )
            )
        return response

    def send_request(self, request):
        response = self.send_request_get_response(request)
        self.print_if_verbose(
            "Raw response from server:\n - HTTP: {}\n - {}\n".format(
                response.http_code, response.raw_response
            )
        )
        coordinate = response.estimated_coordinates
        accuracy = response.loc_accuracy
        self.print_if_verbose(
            "Coordinates: {}\nAccuracy: {}".format(coordinate, accuracy)
        )
        return coordinate

    def build_request_group_iterator_from_result_lines(self, result_lines):
        key_scan_result_groups = FileReader.generate_result_groups(result_lines)
        for key_scan_group in key_scan_result_groups:
            # key = key_scan_group[0]
            group_result_lines = list(key_scan_group[1])
            # date = group_result_lines[0].date
            scan_info = [grp.scan_info for grp in group_result_lines]

            scan_info_types = {scan.__class__ for scan in scan_info}
            if ScannedMacAddress in scan_info_types:
                if ScannedGnss in scan_info_types:
                    raise MultipleScanTypesException()
                else:
                    scan_info_type = ScannedMacAddress
            else:
                if ScannedGnss in scan_info_types:
                    scan_info_type = ScannedGnss
                else:
                    yield group_result_lines, None
                    continue

            if scan_info_type is ScannedMacAddress:
                if self.is_wifi_deactivated():
                    continue
                else:
                    request = self.build_wifi_requests(scan_info)
                yield group_result_lines, request
            elif scan_info_type is ScannedGnss:
                self.configuration.gnss_solver_strategy.get_container().flush()
                for result_line in group_result_lines:
                    self.configuration.gnss_solver_strategy.get_container().push_nav_message(
                        result_line.scan_info
                    )
                    try:
                        request = self.build_gnss_requests_from_strategy()
                    except GroupingMultiFrameNotEnoughNavException as excp:
                        request = None
                    yield [result_line], request
            else:
                continue

    def is_wifi_deactivated(self):
        return self.configuration.deactivate_wifi_requests

    def print_if_verbose(self, output):
        if self.configuration.verbosity:
            print(output)
