"""
Define NAV message library integration class

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

from NavParser import NavMessageConsumerBitsExtractionOutOfBound, NavMessageParser
from lr1110evk.FieldTestPost.Core.FileReader import FileReader
from lr1110evk.BaseTypes import ScannedGnss
from lr1110evk.Job.Logger import ResultLogger
from collections import namedtuple


class FileNavMessageParserException(Exception):
    pass


class NotGnssSolverDestinationException(FileNavMessageParserException):
    def __init__(self, destination):
        self.actual_destination = destination

    def __str__(self):
        return "The NAV message must be destinated to GNSS solver, but its actual destination is {}".format(
            self.actual_destination.get_name()
        )


class FileNavMessageParserInterpreter:
    ParsedGnssNavMessage = namedtuple(
        "ParsedGnssNavMessage", ["date", "job_counter", "job_id", "parsed_info"]
    )

    def __init__(self, input_filename, output_filename=None):
        self.input_filename = input_filename
        if output_filename:
            self.output_logger = ResultLogger(output_filename)
        else:
            self.output_logger = None

    def log_or_print(self, message, date):
        if self.output_logger:
            self.output_logger.log(message, date)
        else:
            print("[{}] {}".format(date, message))

    def log_or_print_metadata(self, message):
        if self.output_logger:
            self.output_logger.log_metadata(message)
        else:
            print(message)

    def parse_file(self):
        scanned_results, version = FileReader.parse_file(self.input_filename)
        self.log_or_print_metadata(version)
        key_scan_result_groups = FileReader.generate_result_groups(scanned_results)
        for key_scan_group in key_scan_result_groups:
            # key = key_scan_group[0]
            group_result_lines = list(key_scan_group[1])
            # date = group_result_lines[0].date
            scan_info = [grp.scan_info for grp in group_result_lines]
            job_id = group_result_lines[0].job_id
            job_counter = group_result_lines[0].job_counter
            scan_info_types = {scan.__class__ for scan in scan_info}
            if not ScannedGnss in scan_info_types:
                continue

            # external_data = [data for data in scan_info if isinstance(
            #     data, ExternalCoordinate)]
            gnss_data = [data for data in scan_info if isinstance(data, ScannedGnss)][0]

            gnss_date = gnss_data.instant_scan
            parsed_nav = NavMessageParser.parse(gnss_data.nav_message)
            try:
                svid_vs_cn = NavMessageParser.get_cn_vs_from_parsed_message(parsed_nav)
            except NotGnssSolverDestinationException:
                continue
            except NavMessageConsumerBitsExtractionOutOfBound:
                #  Unrecognize nav message format
                message_to_log = "[{} - {}] {}".format(
                    job_counter, job_id, ", ".join([gnss_data.nav_message, "0"])
                )
                self.log_or_print(message_to_log, gnss_date)
                message_to_log = "[{} - {}] Failure: bit extraction out of bound".format(
                    job_counter, job_id
                )
                self.log_or_print(message_to_log, gnss_date)
            else:
                message_to_log = "[{} - {}] {}".format(
                    job_counter, job_id, ", ".join([gnss_data.nav_message, "0"])
                )
                self.log_or_print(message_to_log, gnss_date)
                for sv_id, c_n in svid_vs_cn.items():
                    message_to_log = "[{} - {}] SV_id: {}, C/N: {}".format(
                        job_counter, job_id, sv_id, c_n
                    )
                    self.log_or_print(message_to_log, gnss_date)
