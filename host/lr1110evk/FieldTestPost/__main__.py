"""
Define entry point for field test post

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

from argparse import ArgumentParser
from lr1110evk.AppConfiguration import FieldTestConfiguration
from .Core import (
    FileReader,
    RequestSender,
    SolverContactException,
    GeoLocServiceClientGnss,
    GeoLocServiceClientWifi,
    GeoLocServiceBadResponseStatus,
    GeoLocServiceTimeoutException,
    ResponseNoCoordinateException,
    ExternalCoordinate,
)
import pkg_resources
from collections import namedtuple
from .WiFiAnalyzis.WiFiAnalyzis import WiFiAnalyzis
from datetime import timedelta
from datetime import datetime
from lr1110evk.Job.KmlExport import kmlOutput
from lr1110evk import BaseTypes


ResultCoordinate = namedtuple(
    "ResultCoordinate", ["date", "job_counter", "job_id", "coordinate"]
)


class SolverResult:
    def __init__(self, date, job_counter, job_id, information):
        self.date = date
        self.job_counter = job_counter
        self.job_id = job_id
        self._information = information

    @property
    def information(self):
        return self._information


class SolverResultCoordinate(SolverResult):
    def __init__(self, date, job_counter, job_id, coordinate):
        super().__init__(
            date=date, job_counter=job_counter, job_id=job_id, information=coordinate
        )

    @property
    def coordinate(self):
        return self._information

    @property
    def information(self):
        return self.coordinate.to_string()


class SolverExceptionResult(SolverResult):
    def __init__(self, date, job_counter, job_id, exception):
        super().__init__(
            date=date, job_counter=job_counter, job_id=job_id, information=exception
        )

    @property
    def exception(self):
        return self._information

    @property
    def information(self):
        return f"{self.exception}"


def post_analyzis_fetch_results():
    default_wifi_server_port_number = GeoLocServiceClientWifi.get_default_port_number()
    default_gnss_server_port_number = GeoLocServiceClientGnss.get_default_port_number()
    default_wifi_server_base = GeoLocServiceClientWifi.get_default_base_url()
    default_gnss_server_base = GeoLocServiceClientGnss.get_default_base_url()
    default_dry_run = False

    version = pkg_resources.get_distribution("lr1110evk").version
    parser = ArgumentParser()
    parser.add_argument("resultFile", help="File containing the scan results")
    parser.add_argument("outputFile", help="File that will contain the results")
    parser.add_argument(
        "approximateGnssServerLocalization",
        help="approximate coordinate sent to the server as initial localization for GNSS localization solver. Format is <latitude>,<longitude>,<altitude>",
    )
    parser.add_argument(
        "glsAuthenticationToken", help="HTTP header token to authenticate the Wi-Fi requests on LoRa Cloud Geolocation (GLS) server"
    )
    parser.add_argument(
        "dasAuthenticationToken", help="HTTP header token to authenticate the GNSS requests on LoRa Cloud Device and Application Services (DAS) server"
    )
    parser.add_argument(
        "-s",
        "--wifi-server-base-url",
        help="Wifi server base address to use (default={}".format(default_wifi_server_base),
        default=default_wifi_server_base,
    )
    parser.add_argument(
        "-p",
        "--wifi-server-port",
        type=int,
        help="Port number of the wifi server to contact (default={})".format(
            default_wifi_server_port_number
        ),
        default=default_wifi_server_port_number,
    )
    parser.add_argument(
        "-t",
        "--gnss-server-base-url",
        help="GNSS server base address to use (default={}".format(default_gnss_server_base),
        default=default_gnss_server_base,
    )
    parser.add_argument(
        "-q",
        "--gnss-server-port",
        type=int,
        help="Port number of the GNSS server to contact (default={})".format(
            default_gnss_server_port_number
        ),
        default=default_gnss_server_port_number,
    )
    parser.add_argument(
        "-n",
        "--dry-run",
        action="store_true",
        help="Do not contact geolocalization service (default={})".format(
            default_dry_run
        ),
        default=default_dry_run,
    )
    parser.add_argument(
        "--no_wifi",
        "-W",
        help="Deactivate WiFi based location resolution and do not send requests to WiFi server",
        action="store_true",
    )
    parser.add_argument(
        "-u",
        "--user-defined-location",
        help="Define a static location to be used as reference coordinate instead of the one in result file. Format is <latitude>,<longitude>,<altitude>",
        action="store",
    )
    parser.add_argument(
        "-k",
        "--kml-file",
        help="Generate a KML file from the solved location",
        action="store",
        default=None,
    )
    parser.add_argument(
        "--verbose", "-v", help="Verbose", action="store_true", default=False
    )
    parser.add_argument("--version", action="version", version=version)
    args = parser.parse_args()

    configuration = FieldTestConfiguration.from_arg_parser(args)

    request_sender = RequestSender(configuration)
    scan_results, version = FileReader.parse_file(args.resultFile)
    request_sender.device_eui = version.chip_uid
    output_file = args.outputFile

    scan_request_iterator = request_sender.build_request_group_iterator_from_result_lines(
        scan_results, version
    )

    urls_push = {
        "Wi-Fi": configuration.wifi_server.server_address,
        "GNSS": configuration.gnss_server.server_address,
    }
    with open(output_file, "a") as f:
        for request, url in urls_push.items():
            f.write("# {}: {}\n".format(request, url))

    kml_file = args.kml_file

    if kml_file:
        kml = kmlOutput("LR1110", f"{kml_file}")

    for scan_request in scan_request_iterator:
        request = scan_request[1]
        scan_info = scan_request[0]
        init_date = scan_info[0].date
        job_counter = scan_info[0].job_counter
        job_id = scan_info[0].job_id
        if configuration.dry_run:
            if request:
                print(
                    "Dry run: would have send the following on server '{}'\nHeader:\n{}\n\nData:\n{}".format(
                        request_sender.get_geo_loc_service_for_request(
                            request
                        ).server_address,
                        request_sender.get_geo_loc_service_for_request(
                            request
                        ).header,
                        request,
                    )
                )
            continue
        if not request:
            result = SolverExceptionResult(
                date=init_date,
                job_counter=job_counter,
                job_id=job_id,
                exception=f"Exception during scan: '{scan_info}'",
            )
        else:
            try:
                coordinate = request_sender.send_request(request)
            except ResponseNoCoordinateException as excp:
                print("Exception: {}\n  -> Request: {}\n".format(excp, request))
                result = SolverExceptionResult(
                    date=init_date,
                    job_counter=job_counter,
                    job_id=job_id,
                    exception=excp,
                )
            except GeoLocServiceBadResponseStatus as excp_geoloc:
                print("Exception: {}\n  -> Request: {}\n".format(excp_geoloc, request))
                result = SolverExceptionResult(
                    date=init_date,
                    job_counter=job_counter,
                    job_id=job_id,
                    exception=excp_geoloc,
                )
            except GeoLocServiceTimeoutException as excp_timeout:
                print(
                    "Exception: Timeout {}\n  -> Request: {}\n".format(
                        excp_timeout, request
                    )
                )
                result = SolverExceptionResult(
                    date=init_date,
                    job_counter=job_counter,
                    job_id=job_id,
                    exception=excp_timeout,
                )
            except SolverContactException as excp_solver_contact:
                print("Exception: Solver contact failed: '{}'\n  -> Request: {}\n".format(
                        excp_solver_contact, request
                    )
                )
                result = SolverExceptionResult(
                    date=init_date,
                    job_counter=job_counter,
                    job_id=job_id,
                    exception=excp_solver_contact
                )
            else:
                result = SolverResultCoordinate(
                    date=init_date,
                    job_counter=job_counter,
                    job_id=job_id,
                    coordinate=coordinate,
                )
                if kml_file:
                    store_result_to_kml(scan_info, init_date, kml, result)

        with open(output_file, "a") as f:
            f.write(
                "[{result.date}] [{result.job_counter} - {result.job_id}] {result.information}\n".format(
                    result=result,
                )
            )

    if kml_file:
        if configuration.user_defined_coordinate:
            kml.add_point(
                "User defined",
                kmlOutput.SCAN_TYPE_REFERENCE_COORDINATES,
                configuration.user_defined_coordinate,
            )
        kml.save()


def store_result_to_kml(scan_info, date, kml: kmlOutput, result):
    data = {}
    date_str = date.strftime("%y/%m/%d - %H:%M:%S")
    for scan in scan_info:
        if type(scan.scan_info) == BaseTypes.ScannedMacAddress:
            data[kmlOutput.SCAN_TYPE_WIFI] = data.get(kmlOutput.SCAN_TYPE_WIFI, [])
            data[kmlOutput.SCAN_TYPE_WIFI].append(scan.scan_info)
        elif type(scan.scan_info) == BaseTypes.ScannedGnss:
            data[kmlOutput.SCAN_TYPE_GNSS] = scan.scan_info
        elif type(scan.scan_info) == ExternalCoordinate:
            kml.add_point(
                date_str,
                kmlOutput.SCAN_TYPE_REFERENCE_COORDINATES,
                scan.scan_info.coordinate,
                date_str,
            )
            # data[kmlOutput.SCAN_TYPE_REFERENCE_COORDINATES] = f"{scan.scan_info.external_id}: {scan.scan_info.coordinate}"
        elif type(scan.scan_info) == str:
            # This situation occurs on malformed externals
            # In that case we simply don't store kml info for this malformed external
            pass
        else:
            raise NotImplementedError(
                f'Result type "{type(scan.scan_info)}" is not implemented (value is "{scan.scan_info}")'
            )

    for scan_type, metadata in data.items():
        kml.add_point(date_str, scan_type, result.coordinate, metadata)


def wifi_analyzis():
    version = pkg_resources.get_distribution("lr1110evk").version
    parser = ArgumentParser()
    parser.add_argument("resultFile", help="File containing the scan results")
    parser.add_argument("--version", action="version", version=version)
    args = parser.parse_args()

    wifi_analyzis = WiFiAnalyzis()
    wifi_analyzis.parse_measurement_file(args.resultFile)
    count_per_time = wifi_analyzis.mac_addresses_count_over_time()

    time_window = timedelta(minutes=20)
    average_count = WiFiAnalyzis.compute_sliding_average_per_time(
        count_per_time, time_window
    )


if __name__ == "__main__":
    # wifi_analyzis()
    post_analyzis_fetch_results()
