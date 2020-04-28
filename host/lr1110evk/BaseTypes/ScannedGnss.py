"""
Define GNSS Scan result class

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

from datetime import timedelta
import re


class ScannedGnssException(Exception):
    """Base exception for ScannedGnss module

    """

    pass


class ScannedGnssMalformedCsvLineException(ScannedGnssException):
    """ Exception describing the failure to interpret a CSV-like line
    as GNSS result

    Attributes:
        wrong_line (str): The line that failed to be interpreted as
            ScannedGnss object

    """

    def __init__(self, line):
        self.wrong_line = line

    def __str__(self):
        return "Cannot convert CSV line to scanned GNSS: '{}'".format(self.wrong_line)


class GnssSystemException(ScannedGnssException):
    pass


class GnssUnknownSystemException(GnssSystemException):
    def __init__(self, unknown_system):
        self.unknown_system = unknown_system

    def __str__(self):
        return "Unkown GNSS system '{}'".format(self.unknown_system)


class GnssSystemBase:
    KEY = None

    def __str__(self):
        return self.KEY


class GnssSystemGPS(GnssSystemBase):
    KEY = "GPS"


class GnssSystemBuilder:
    STRING_TYPE_MAPPER = {"GPS": GnssSystemGPS}

    @staticmethod
    def from_string(value):
        try:
            gnss_type = GnssSystemBuilder.STRING_TYPE_MAPPER[value.upper()]()
        except KeyError:
            raise GnssUnknownSystemException(value)
        else:
            return gnss_type


class ScannedGnss:
    """ Describes a GNSS signal discovered by LR1110

    This class describe objects that hold meta-data
    associated to a GNSS discovered by LR1110.

    Attributes:
        gps_number (str): The GPS ID reported
        coarse_timestamp (WifiChannel): The coarse timestamp reported
        fine_timestamp (str): The fine timestamp reported
        frequency_offset (int): The frequency offset reported
        snr (int): The snr reported

    """

    regexp_csv = re.compile(
        "^(?P<nav_message>[A-Za-z0-9]+), ?(?P<elapsed_s>[0-9]+), ?(?P<radio_timing_ms>[0-9]+), ?(?P<computation_timing_ms>[0-9]+)$"
    )

    def __init__(
        self, nav_message, instant_scan, radio_timing_ms, computation_timing_ms
    ):
        self.__nav_message = nav_message
        self.__instant_scan = instant_scan
        self.__radio_timing_ms = radio_timing_ms
        self.__computation_timing_ms = computation_timing_ms

    @property
    def nav_message(self):
        return self.__nav_message

    @property
    def instant_scan(self):
        return self.__instant_scan

    @property
    def radio_timing_ms(self):
        return self.__radio_timing_ms

    @property
    def computation_timing_ms(self):
        return self.__computation_timing_ms

    @staticmethod
    def from_bytes(raw_bytes, instant_scan_received):
        # 1. Local measurement delay
        elapsed_time = int.from_bytes(raw_bytes[0:4], byteorder="little")
        delta = timedelta(milliseconds=elapsed_time)
        instant_scan = instant_scan_received - delta

        # 2. Radio timings
        radio_timing_ms = int.from_bytes(raw_bytes[4:8], byteorder="little")
        computation_timing_ms = int.from_bytes(raw_bytes[8:12], byteorder="little")

        # 3. NAV message
        nav_message = raw_bytes[12:].hex()

        gnss = ScannedGnss(
            nav_message=nav_message,
            instant_scan=instant_scan,
            radio_timing_ms=radio_timing_ms,
            computation_timing_ms=computation_timing_ms,
        )
        return gnss

    @staticmethod
    def from_csv_line(date_scan, csv_line):
        """ Factory method from a CSV-like line

        This factory method try to build a ScannedGnss object out of
        a CSV line. The expected schema of the CSV line is:
        '<NAV_MESSAGE>,<ELAPSED_TIME_CAPTURE_PRINT> (ms)'

        A ScannedGnssMalformedCsvLineException exception is raised if
        the line does not respect the expected formatting.

        Args:
            csv_line (str): The line to interpret as a ScannedGnss object

        """
        match_mac_line = ScannedGnss.regexp_csv.match(csv_line)
        if match_mac_line:
            nav_message = match_mac_line.group("nav_message")
            radio_timing_ms = match_mac_line.group("radio_timing_ms")
            computation_timing_ms = match_mac_line.group("computation_timing_ms")
            elapsed_time = int(match_mac_line.group("elapsed_s"))
            delta = timedelta(seconds=elapsed_time)
            instant_scan = date_scan - delta
            instant_scan_corrected = instant_scan.replace(microsecond=0)

            gnss = ScannedGnss(
                nav_message=nav_message,
                instant_scan=instant_scan_corrected,
                radio_timing_ms=radio_timing_ms,
                computation_timing_ms=computation_timing_ms,
            )
            return gnss
        else:
            raise ScannedGnssMalformedCsvLineException(csv_line)

    def __str__(self):
        return "GNSS {} - radio timing: {} ms - computation timing: {} ms - {}".format(
            self.nav_message,
            self.radio_timing_ms,
            self.computation_timing_ms,
            self.instant_scan,
        )

    def __repr__(self):
        return "ScannedGnss<{}, {} ms, {} ms, {}>".format(
            self.nav_message,
            self.radio_timing_ms,
            self.computation_timing_ms,
            self.instant_scan,
        )
