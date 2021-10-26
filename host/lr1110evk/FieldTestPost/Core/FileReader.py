"""
Define result file reader class

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

import re
from datetime import datetime
from lr1110evk.BaseTypes import (
    ScannedGnss,
    ScannedMacAddress,
    ScannedMacAddressMalformedCsvLineException,
    Version,
    ScannedGnssMalformedCsvLineException,
)
from collections import namedtuple
from itertools import groupby
from lr1110evk.Job import ResultLogger
from lr1110evk.FieldTestPost.Core.ExternalCoordinate import (
    ExternalCoordinate,
    ExternalCoordinateMalformedCsvLineException,
)


ResultLine = namedtuple("ResultLine", ["date", "job_counter", "job_id", "scan_info"])


class FileReader:
    DATE_FORMAT = "%Y-%m-%d %H:%M:%S.%f"
    VERSION_TOKEN = "# version:"
    REGEXP_LOG = re.compile(
        "^\[(?P<date_info>[^\]]+)\] \[(?P<job_counter>[0-9]+) - (?P<job_id>[0-9]+)\] ?(?P<scan_info>.*)$"
    )

    def __init__(self):
        pass

    @staticmethod
    def parse_file(filename):
        scanned_results = list()
        version = None
        with open(filename, "r") as f:
            lines = f.readlines()
        for line in lines:
            if line.startswith(FileReader.VERSION_TOKEN):
                version = Version.from_metadata(line[len(FileReader.VERSION_TOKEN) :])
            else:
                try:
                    result_line = FileReader.get_scan_info_and_job_data_from_line(line)
                except Exception as malformed_except:
                    print(
                        "Failed to parse line: {} ('{}')".format(line, malformed_except)
                    )
                    result_line = None
                if result_line:
                    scanned_results.append(result_line)
        if not version:
            version = Version(
                host_version="unknown",
                demo_version="unknown",
                driver_version="unknown",
                lr1110_version="unknown",
                almanac_crc="unknown",
                almanac_ages="unknown",
                chip_uid=FileReader.DEFAULT_CHIP_UID_VERSION,
            )
        return scanned_results, version

    @staticmethod
    def get_scan_info_and_job_data_from_line(str_line):
        match_line = FileReader.REGEXP_LOG.match(str_line)
        date_scan_str = match_line.group("date_info")
        job_counter = int(match_line.group("job_counter"))
        job_id = int(match_line.group("job_id"))
        scan_line = match_line.group("scan_info")
        date_scan = datetime.strptime(date_scan_str, FileReader.DATE_FORMAT)
        if (
            scan_line == ResultLogger.NO_RESULT_TOKEN
            or scan_line == ResultLogger.EXCEPTION_TOKEN
        ):
            scan = "Scan exception: {}".format(scan_line)
        else:
            try:
                scan = ScannedMacAddress.from_csv_line(date_scan, scan_line)
            except ScannedMacAddressMalformedCsvLineException:
                try:
                    scan = ScannedGnss.from_csv_line(date_scan, scan_line)
                except ScannedGnssMalformedCsvLineException:
                    try:
                        scan = ExternalCoordinate.from_csv_line(scan_line)
                    except ExternalCoordinateMalformedCsvLineException as malformed_exc:
                        scan = "Malformed result: {}".format(
                            malformed_exc.malformed_csv_line
                        )
        result_line = ResultLine(
            date=date_scan, job_counter=job_counter, job_id=job_id, scan_info=scan
        )
        return result_line

    @staticmethod
    def generate_result_groups(result_lines):
        def group_key(res):
            group_val = res.job_counter
            return group_val

        sorted_result_lines = sorted(result_lines, key=group_key)
        key_scan_result_groups = groupby(sorted_result_lines, group_key)
        return key_scan_result_groups
