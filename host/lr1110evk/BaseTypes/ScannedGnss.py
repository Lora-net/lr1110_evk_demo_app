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
from dataclasses import dataclass
import re


class ScannedGnssException(Exception):
    """Base exception for ScannedGnss module"""

    pass


class ScannedGnssMalformedCsvLineException(ScannedGnssException):
    """Exception describing the failure to interpret a CSV-like line
    as GNSS result

    Attributes:
        wrong_line (str): The line that failed to be interpreted as
            ScannedGnss object

    """

    def __init__(self, line):
        self.wrong_line = line

    def __str__(self):
        return "Cannot convert CSV line to scanned GNSS: '{}'".format(self.wrong_line)


class ScannedGnssMalformedByteStream(ScannedGnssException):
    """Exception describing the failure to parse a byte stream that is supposed to
    correspond to a GNSS result

    Attributes:
        wrong_byte_stream (bytes): The byte array that failed to be interpreted as
            ScannedGnss object

    """

    def __init__(self, byte_stream: bytes):
        self.wrong_byte_stream = byte_stream

    def __str__(self):
        return "Cannot convert byte stream to scanned GNSS: '{}'".format(
            self.wrong_byte_stream.hex()
        )


class GnssSystemException(ScannedGnssException):
    pass


class GnssUnknownSystemException(GnssSystemException):
    def __init__(self, unknown_system):
        self.unknown_system = unknown_system

    def __str__(self):
        return "Unkown GNSS system '{}'".format(self.unknown_system)


class GnssSystemBase:
    KEY = None
    VALUE = None

    def __str__(self):
        return self.KEY

    @classmethod
    def __eq__(cls, other):
        return isinstance(other, cls)


class GnssSystemGPS(GnssSystemBase):
    KEY = "GPS"
    VALUE = b"\x01"


class GnssSystemBeiDou(GnssSystemBase):
    KEY = "BeiDou"
    VALUE = b"\x02"


class GnssSystemBuilder:
    STRING_TYPE_MAPPER = {
        system.KEY: system for system in [GnssSystemGPS, GnssSystemBeiDou]
    }
    VALUE_TYPE_MAPPER = {
        system.VALUE: system for system in [GnssSystemGPS, GnssSystemBeiDou]
    }

    @staticmethod
    def from_key(key):
        try:
            gnss_type = GnssSystemBuilder.STRING_TYPE_MAPPER[key]()
        except KeyError:
            raise GnssUnknownSystemException(key)
        else:
            return gnss_type

    @staticmethod
    def from_value(value):
        try:
            gnss_type = GnssSystemBuilder.VALUE_TYPE_MAPPER[value]()
        except KeyError:
            raise GnssUnknownSystemException(value)
        else:
            return gnss_type


@dataclass
class SatelliteDetail:
    satellite_id: int
    snr: float
    constellation: GnssSystemBase


class ScannedGnss:
    """Describes a GNSS signal discovered by LR1110

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
        "^(?P<nav_message>[A-Za-z0-9]+), ?(?P<elapsed_s>[0-9]+), ?(?P<radio_timing_ms>[0-9]+), ?(?P<computation_timing_ms>[0-9]+), ?(?P<satellite_details>[^,]*)$"
    )

    def __init__(
        self,
        nav_message,
        instant_scan,
        radio_timing_ms,
        computation_timing_ms,
        satellite_details,
    ):
        self.__nav_message = nav_message
        self.__instant_scan = instant_scan
        self.__radio_timing_ms = radio_timing_ms
        self.__computation_timing_ms = computation_timing_ms
        self.__satellite_details = satellite_details

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

    @property
    def satellite_details(self):
        return self.__satellite_details

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
        nav_message_length = int.from_bytes(raw_bytes[12:14], byteorder="little")
        nav_message = raw_bytes[14 : 14 + nav_message_length].hex()

        # 4. Satellite details
        # The remaining length must be a multiple of 4
        remaining_raw_bytes = bytearray(raw_bytes[14 + nav_message_length :])
        if (len(remaining_raw_bytes) % 4) != 0:
            raise ScannedGnssMalformedByteStream(raw_bytes)

        number_of_satellites = len(remaining_raw_bytes) // 4
        satellites = list()
        while remaining_raw_bytes:
            s_id = remaining_raw_bytes[0]
            s_constellation = GnssSystemBuilder.from_value(
                remaining_raw_bytes[1].to_bytes(1, byteorder="little")
            )
            s_snr = int.from_bytes(
                remaining_raw_bytes[2:4], byteorder="little", signed=True
            )
            remaining_raw_bytes = remaining_raw_bytes[4:]
            satellites.append(
                SatelliteDetail(
                    satellite_id=s_id, snr=s_snr, constellation=s_constellation
                )
            )

        gnss = ScannedGnss(
            nav_message=nav_message,
            instant_scan=instant_scan,
            radio_timing_ms=radio_timing_ms,
            computation_timing_ms=computation_timing_ms,
            satellite_details=satellites,
        )
        return gnss

    @staticmethod
    def from_csv_line(date_scan, csv_line):
        """Factory method from a CSV-like line

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

            satellite_details = list()
            satelite_details_raw = match_mac_line.group("satellite_details")
            if satelite_details_raw:
                for sat in satelite_details_raw.split("|"):
                    try:
                        s_id, s_gnss, s_snr = sat.split("+")
                    except ValueError as excp:
                        raise ScannedGnssMalformedCsvLineException(csv_line) from excp
                    satellite_details.append(
                        SatelliteDetail(
                            satellite_id=int(s_id),
                            constellation=GnssSystemBuilder.from_key(s_gnss),
                            snr=float(s_snr),
                        )
                    )

            gnss = ScannedGnss(
                nav_message=nav_message,
                instant_scan=instant_scan_corrected,
                radio_timing_ms=radio_timing_ms,
                computation_timing_ms=computation_timing_ms,
                satellite_details=satellite_details,
            )
            return gnss
        else:
            raise ScannedGnssMalformedCsvLineException(csv_line)

    def to_csv_line(self):
        satellites_str = "|".join(
            [
                "+".join([str(sat.satellite_id), str(sat.constellation), str(sat.snr)])
                for sat in self.satellite_details
            ]
        )
        return f"{self.nav_message}, 0, {self.radio_timing_ms}, {self.computation_timing_ms}, {satellites_str}"

    def __str__(self):
        return "GNSS {} - radio timing: {} ms - computation timing: {} ms - {} - satellite details: {}".format(
            self.nav_message,
            self.radio_timing_ms,
            self.computation_timing_ms,
            self.instant_scan,
            ", ".join(
                "{}{} {}dB".format(sat.constellation, sat.satellite_id, sat.snr)
                for sat in self.satellite_details
            ),
        )

    def __repr__(self):
        return "ScannedGnss<{}, {} ms, {} ms, {}>".format(
            self.nav_message,
            self.radio_timing_ms,
            self.computation_timing_ms,
            self.instant_scan,
        )
