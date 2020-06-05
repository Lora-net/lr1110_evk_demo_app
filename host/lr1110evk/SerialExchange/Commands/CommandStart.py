"""
Define Start serial command class

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

from lr1110evk.BaseTypes import WifiChannels
from .CommandBase import CommandBase
from enum import Enum, unique


@unique
class GnssOption(Enum):
    default = b"\x00"
    best_effort = b"\x01"


@unique
class GnssCaptureMode(Enum):
    single = b"\x00"
    dual = b"\x01"


@unique
class GnssAntennaSelection(Enum):
    no_selection = b"\x00"
    select_antenna_1 = b"\x01"
    select_antenna_2 = b"\x02"


@unique
class GnssConstellation(Enum):
    gps = b"\x01"
    beidou = b"\x02"


@unique
class WifiMode(Enum):
    beacon_only = b"\x01"
    beacon_and_packet = b"\x02"


@unique
class WifiEnableMode(Enum):
    disabled = b"\x00"
    wifi_scan = b"\x01"
    country_code = b"\x02"


class CommandStart(CommandBase):
    DEMO_ID = ""

    def __init__(self):
        pass

    @staticmethod
    def get_com_code():
        return b"\x01\x00"

    @classmethod
    def get_demo_id(cls):
        return cls.DEMO_ID

    def config_payload_to_byte(self):
        raise NotImplementedError

    def payload_to_bytes(self):
        return self.get_demo_id() + self.config_payload_to_byte()


class CommandStartWifiBase(CommandStart):
    def __init__(self):
        self.wifi_channels = list()
        self.wifi_nbr_retrials = None
        self.wifi_max_results_per_scan = None
        self.wifi_timeout = None

    @staticmethod
    def channel_list_to_bit_mask(channel_list):
        channels_values_mapper = {
            channel: (1 << (int(channel.name.split("_")[1]) - 1))
            for channel in WifiChannels.WIFI_CHANNELS
        }
        bit_mask = sum(
            [channels_values_mapper[channel] for channel in channel_list]
        ).to_bytes(2, "little")
        return bit_mask

    @staticmethod
    def type_list_to_bit_mask(type_list):
        types_value_mapper = {"TYPE_B": 1, "TYPE_G": 2}
        bit_mask = sum([types_value_mapper[type_] for type_ in type_list]).to_bytes(
            1, "little"
        )
        return bit_mask


class CommandStartWifiScan(CommandStartWifiBase):
    DEMO_ID = b"\x01"

    def __init__(self):
        super().__init__()
        self.wifi_types = list()
        self.wifi_mode = None

    def config_payload_to_byte(self):
        wifi_channel_mask_bytes = CommandStartWifiBase.channel_list_to_bit_mask(
            self.wifi_channels
        )
        wifi_type_mask_byte = CommandStartWifiBase.type_list_to_bit_mask(
            self.wifi_types
        )
        wifi_nbr_retrials_bytes = self.wifi_nbr_retrials.to_bytes(1, byteorder="little")
        wifi_max_results_bytes = self.wifi_max_results_per_scan.to_bytes(
            1, byteorder="little"
        )
        wifi_timeout_bytes = self.wifi_timeout.to_bytes(2, byteorder="little")
        wifi_mode_byte = self.wifi_mode.value

        return (
            wifi_channel_mask_bytes
            + wifi_type_mask_byte
            + wifi_nbr_retrials_bytes
            + wifi_max_results_bytes
            + wifi_timeout_bytes
            + wifi_mode_byte
        )


class CommandStartWifiCountryCode(CommandStartWifiBase):
    DEMO_ID = b"\x02"

    def __init__(self):
        super().__init__()

    def config_payload_to_byte(self):
        wifi_channel_mask_bytes = CommandStartWifiBase.channel_list_to_bit_mask(
            self.wifi_channels
        )
        wifi_nbr_retrials_bytes = self.wifi_nbr_retrials.to_bytes(1, byteorder="little")
        wifi_max_results_bytes = self.wifi_max_results_per_scan.to_bytes(
            1, byteorder="little"
        )
        wifi_timeout_bytes = self.wifi_timeout.to_bytes(2, byteorder="little")

        return (
            wifi_channel_mask_bytes
            + wifi_nbr_retrials_bytes
            + wifi_max_results_bytes
            + wifi_timeout_bytes
        )


class CommandStartGnssBase(CommandStart):
    def __init__(self):
        self.gnss_option = None
        self.gnss_capture_mode = None
        self.gnss_nb_satellite = None
        self.gnss_antenna_selection = None
        self.gnss_constellation_mask = None

    def config_payload_to_byte(self):
        gnss_option = self.gnss_option.value
        gnss_capture_mode = self.gnss_capture_mode.value
        gnss_nb_satellite = self.gnss_nb_satellite.to_bytes(1, byteorder="little")
        gnss_antenna_selection = self.gnss_antenna_selection.value
        gnss_constellation_mask = self.gnss_constellation_mask

        return (
            gnss_option
            + gnss_capture_mode
            + gnss_nb_satellite
            + gnss_antenna_selection
            + gnss_constellation_mask
        )


class CommandStartGnssAutonomous(CommandStartGnssBase):
    DEMO_ID = b"\x03"


class CommandStartGnssAssisted(CommandStartGnssBase):
    DEMO_ID = b"\x04"
