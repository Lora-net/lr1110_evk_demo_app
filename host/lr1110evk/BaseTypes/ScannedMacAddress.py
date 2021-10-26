"""
Define Wi-Fi MAC address scan result class

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

from .WifiChannels import WifiChannels
import re


class ScannedMacAddressException(Exception):
    """Base exception for ScannedMacAddress module"""

    pass


class ScannedMacAddressMalformedCsvLineException(ScannedMacAddressException):
    """Exception describing the failure to interpret a CSV-like line
    as ScannedMacAddress

    Attributes:
        wrong_line (str): The line that failed to be interpreted as
            ScannedMacAddress object

    """

    def __init__(self, line):
        self.wrong_line = line

    def __str__(self):
        return "Cannot convert CSV line to scanned mac address: '{}'".format(
            self.wrong_line
        )


class ScannedMacAddress:
    """Describes a MAC address discovered by LR1110

    This class describe immutable objects that hold meta-data
    associated to a MAC address discovered by LR1110.
    It defines the necessary methods to compare two of its objects
    based on actual MAC address, channel and wifi type

    Attributes:
        mac_address (str): The actual MAC address
        wifi_channel (WifiChannel): The Wifi Channel object on which
            the MAC address has been discovered
        wifi_type (str): The wifi type reported
        rssi (int): The RSSI [dBm] reported

    """

    regexp_csv = re.compile(
        "^(?P<mac_address>[A-Za-z0-9:]+), ?(?P<channel>CHANNEL_[0-9]+), ?(?P<wifi_type>TYPE_[BG]), ?(?P<rssi>-?[0-9]+), ?(?P<timing_demodulation>[0-9]+), ?(?P<timing_capture>[0-9]+), ?(?P<timing_correlation>[0-9]+), ?(?P<timing_detection>[0-9]+)$"
    )

    def __init__(
        self,
        mac_address,
        wifi_channel,
        wifi_type,
        rssi,
        timing_demodulation,
        timing_capture,
        timing_correlation,
        timing_detection,
        instant_scan,
    ):
        self.__mac_address = mac_address
        self.__rssi = rssi
        self.__wifi_channel = wifi_channel
        self.__wifi_type = wifi_type
        self.__timing_demodulation = timing_demodulation
        self.__timing_capture = timing_capture
        self.__timing_correlation = timing_correlation
        self.__timing_detection = timing_detection
        self.__instant_scan = instant_scan

    @property
    def mac_address(self):
        return self.__mac_address

    @property
    def wifi_channel(self):
        return self.__wifi_channel

    @property
    def wifi_type(self):
        return self.__wifi_type

    @property
    def rssi(self):
        return self.__rssi

    @property
    def timing_demodulation(self):
        return self.__timing_demodulation

    @property
    def timing_capture(self):
        return self.__timing_capture

    @property
    def timing_correlation(self):
        return self.__timing_correlation

    @property
    def timing_detection(self):
        return self.__timing_detection

    @property
    def instant_scan(self):
        return self.__instant_scan

    @staticmethod
    def from_csv_line(date_scan, csv_line):
        """Factory method from a CSV-like line

        This factory method try to build a ScannedMacAddress object out of
        a CSV line. The expected schema of the CSV line is:
        '<MAC-ADDRESS>,<CHANNEL-NAME>,<WIFI-TYPE-NAME>,<RSSI>,<TIMING_DEMODULATION>,<TIMING_CAPTURE>,<TIMING_CORRELATION>,<TIMING_DETECTION>'

        A ScannedMacAddressMalformedCsvLineException exception is raised if
        the line does not respect the expected formatting.

        Args:
            csv_line (str): The line to interpret as a ScannedMacAddress object

        """
        match_mac_line = ScannedMacAddress.regexp_csv.match(csv_line)
        if match_mac_line:
            mac_address = match_mac_line.group("mac_address")
            channel = WifiChannels.get_channel_from_name(
                match_mac_line.group("channel")
            )
            wifi_type = match_mac_line.group("wifi_type")
            rssi = int(match_mac_line.group("rssi"))
            timing_demodulation = int(match_mac_line.group("timing_demodulation"))
            timing_capture = int(match_mac_line.group("timing_capture"))
            timing_correlation = int(match_mac_line.group("timing_correlation"))
            timing_detection = int(match_mac_line.group("timing_detection"))
            mac = ScannedMacAddress(
                mac_address,
                channel,
                wifi_type,
                rssi,
                timing_demodulation,
                timing_capture,
                timing_correlation,
                timing_detection,
                date_scan,
            )
            return mac
        else:
            raise ScannedMacAddressMalformedCsvLineException(csv_line)

    @staticmethod
    def from_bytes(raw_bytes, instant_scan_received):
        WifiTypeValuNameMapper = {
            1: "TYPE_B",
            2: "TYPE_G",
        }

        mac_address = ":".join(["{:02x}".format(mm) for mm in raw_bytes[0:6]])
        channel = WifiChannels.WIFI_CHANNELS[raw_bytes[6] - 1]
        wifi_type = WifiTypeValuNameMapper[raw_bytes[7]]
        rssi = int.from_bytes(
            raw_bytes[8].to_bytes(length=1, byteorder="little"),
            byteorder="little",
            signed=True,
        )
        detection_time = int.from_bytes(raw_bytes[9:13], byteorder="little")
        correlation_time = int.from_bytes(raw_bytes[13:17], byteorder="little")
        capture_time = int.from_bytes(raw_bytes[17:21], byteorder="little")
        demodulation_time = int.from_bytes(raw_bytes[21:25], byteorder="little")
        instant_scan = instant_scan_received
        mac_address = ScannedMacAddress(
            mac_address=mac_address,
            wifi_channel=channel,
            wifi_type=wifi_type,
            rssi=rssi,
            timing_demodulation=demodulation_time,
            timing_capture=capture_time,
            timing_correlation=correlation_time,
            timing_detection=detection_time,
            instant_scan=instant_scan,
        )
        return mac_address

    def __str__(self):
        return "MAC {} ({}, {}, {} dBm)".format(
            self.mac_address, self.wifi_channel.name, self.wifi_type, self.rssi
        )

    def __repr__(self):
        return "ScannedMacAddress<{}, {}, {}, {} dBm>".format(
            self.mac_address, self.wifi_channel.name, self.wifi_type, self.rssi
        )
