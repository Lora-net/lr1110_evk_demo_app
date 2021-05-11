"""
Define Field test job class

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

from ..SerialExchange.Commands import (
    GnssOption,
    GnssCaptureMode,
    WifiMode,
    WifiEnableMode,
    GnssAntennaSelection,
    GnssConstellation,
)
from ..BaseTypes import Coordinate, WifiChannels
from copy import copy


class Job:
    WIFI_CHANNELS_KEY = "wifi_channels"
    WIFI_TYPES_KEY = "wifi_types"
    WIFI_API_KEY = "wifi_api"
    WIFI_NBR_RETRIAL_KEY = "wifi_nbr_retrial"
    WIFI_MAX_RESULT_PER_SCAN_KEY = "wifi_max_result_per_scan"
    WIFI_TIMEOUT_KEY = "wifi_timeout"
    WIFI_MODE_KEY = "wifi_mode"
    WIFI_ABORT_ON_TIMEOUT_KEY = "wifi_abort_on_timeout"
    GNSS_AUTONOMOUS_ENABLE_KEY = "gnss_autonomous_enable"
    GNSS_AUTONOMOUS_OPTION_KEY = "gnss_autonomous_option"
    GNSS_AUTONOMOUS_CAPTURE_MODE_KEY = "gnss_autonomous_capture_mode"
    GNSS_AUTONOMOUS_NB_SATELLITE_KEY = "gnss_autonomous_nb_satellite"
    GNSS_AUTONOMOUS_ANTENNA_SELECTION = "gnss_autonomous_antenna_selection"
    GNSS_AUTONOMOUS_CONSTELLATIONS_KEY = "gnss_autonomous_constellations"
    GNSS_ASSISTED_ENABLE_KEY = "gnss_assisted_enable"
    GNSS_ASSISTED_OPTION_KEY = "gnss_assisted_option"
    GNSS_ASSISTED_CAPTURE_MODE_KEY = "gnss_assisted_capture_mode"
    GNSS_ASSISTED_NB_SATELLITE_KEY = "gnss_assisted_nb_satellite"
    GNSS_ASSISTED_ANTENNA_SELECTION = "gnss_assisted_antenna_selection"
    GNSS_ASSISTED_CONSTELLATIONS_KEY = "gnss_assisted_constellations"
    ASSISTED_COORDINATE_KEY = "assisted_coordinate"
    LATITUDE_KEY = "latitude"
    LONGITUDE_KEY = "longitude"
    ALTITUDE_KEY = "altitude"
    JOB_NAME_KEY = "name"
    N_ITERATIONS_KEY = "n_iterations"
    RESET_BEFORE_JOB_START_KEY = "reset_before_job_start"

    def __init__(
        self,
        name,
        n_iteration,
        reset_before_start,
        gnss_autonomous_enable,
        gnss_assisted_enable,
    ):
        self.wifi_channels = list()
        self.wifi_types = ["TYPE_B"]
        self.wifi_enable_mode = WifiEnableMode.disabled
        self.wifi_nbr_retrials = 0
        self.wifi_max_results = 0
        self.wifi_timeout = 0
        self.wifi_mode = WifiMode.beacon_only
        self.wifi_abort_on_timeout = False
        self.gnss_autonomous_enable = gnss_autonomous_enable
        self.gnss_autonomous_option = GnssOption.default
        self.gnss_autonomous_capture_mode = GnssCaptureMode.single
        self.gnss_autonomous_nb_satellite = 0
        self.gnss_autonomous_antenna_selection = GnssAntennaSelection.no_selection
        self.gnss_autonomous_constellation_mask = b"\x00"
        self.gnss_assisted_enable = gnss_assisted_enable
        self.gnss_assisted_option = GnssOption.default
        self.gnss_assisted_capture_mode = GnssCaptureMode.single
        self.gnss_assisted_nb_satellite = 0
        self.gnss_assisted_antenna_selection = GnssAntennaSelection.no_selection
        self.gnss_assisted_constellation_mask = b"\x00"
        self.assisted_coordinate = Coordinate(0, 0, 0)
        self.id = 0
        self.name = name
        self.n_iterations = n_iteration
        self.reset_before_job_start = reset_before_start

    def SetCoordinatesFromJobDict(self, coordinate_dict):
        assisted_coordinate = Coordinate(
            latitude=coordinate_dict[Job.LATITUDE_KEY],
            longitude=coordinate_dict[Job.LONGITUDE_KEY],
            altitude=coordinate_dict[Job.ALTITUDE_KEY],
        )
        self.assisted_coordinate = assisted_coordinate

    def SetWifiChannelsFromJobDict(self, wifi_channel_names):
        self.wifi_channels = [
            WifiChannels.get_channel_from_name(name) for name in wifi_channel_names
        ]

    def SetWifiEnableMode(self, wifi_enable_mode):
        self.wifi_enable_mode = WifiEnableMode[wifi_enable_mode]

    def SetWifiModeFromJobDict(self, wifi_mode):
        self.wifi_mode = WifiMode[wifi_mode]

    def SetGnssAutonomousOptionFromJobDict(self, autonomous_option):
        self.gnss_autonomous_option = GnssOption[autonomous_option]

    def SetGnssAutonomousCaptureModeFromJobDict(self, capture_mode):
        self.gnss_autonomous_capture_mode = GnssCaptureMode[capture_mode]

    def SetGnssAutonomousAntennaSelectionFromJobDict(self, antenna_selection):
        self.gnss_autonomous_antenna_selection = GnssAntennaSelection[antenna_selection]

    def SetGnssAutonomousConstellationMaskFromJobDict(self, constellations):
        self.gnss_autonomous_constellation_mask = Job.constellations_to_mask(
            constellations
        )

    def SetGnssAssistedOptionFromJobDict(self, assisted_option):
        self.gnss_assisted_option = GnssOption[assisted_option]

    def SetGnssAssistedCaptureModeFromJobDict(self, capture_mode):
        self.gnss_assisted_capture_mode = GnssCaptureMode[capture_mode]

    def SetGnssAssistedAntennaSelectionFromJobDict(self, antenna_selection):
        self.gnss_assisted_antenna_selection = GnssAntennaSelection[antenna_selection]

    def SetGnssAssistedConstellationMaskFromJobDict(self, constellations):
        self.gnss_assisted_constellation_mask = Job.constellations_to_mask(
            constellations
        )

    def SetJobElement(self, element_key, element_value):
        SETTERS = {
            Job.WIFI_API_KEY: lambda obj, value: Job.SetWifiEnableMode(obj, value),
            Job.WIFI_CHANNELS_KEY: lambda obj, value: Job.SetWifiChannelsFromJobDict(
                obj, value
            ),
            Job.WIFI_TYPES_KEY: lambda obj, value: setattr(obj, "wifi_types", value),
            Job.WIFI_NBR_RETRIAL_KEY: lambda obj, value: setattr(
                obj, "wifi_nbr_retrials", value
            ),
            Job.WIFI_MAX_RESULT_PER_SCAN_KEY: lambda obj, value: setattr(
                obj, "wifi_max_results", value
            ),
            Job.WIFI_TIMEOUT_KEY: lambda obj, value: setattr(
                obj, "wifi_timeout", value
            ),
            Job.WIFI_MODE_KEY: lambda obj, value: Job.SetWifiModeFromJobDict(
                obj, value
            ),
            Job.WIFI_ABORT_ON_TIMEOUT_KEY: lambda obj, value: setattr(
                obj, "wifi_abort_on_timeout", value
            ),
            Job.GNSS_AUTONOMOUS_OPTION_KEY: lambda obj, value: Job.SetGnssAutonomousOptionFromJobDict(
                obj, value
            ),
            Job.GNSS_AUTONOMOUS_CAPTURE_MODE_KEY: lambda obj, value: Job.SetGnssAutonomousCaptureModeFromJobDict(
                obj, value
            ),
            Job.GNSS_AUTONOMOUS_NB_SATELLITE_KEY: lambda obj, value: setattr(
                obj, "gnss_autonomous_nb_satellite", value
            ),
            Job.GNSS_AUTONOMOUS_ANTENNA_SELECTION: lambda obj, value: Job.SetGnssAutonomousAntennaSelectionFromJobDict(
                obj, value
            ),
            Job.GNSS_AUTONOMOUS_CONSTELLATIONS_KEY: lambda obj, value: Job.SetGnssAutonomousConstellationMaskFromJobDict(
                obj, value
            ),
            Job.GNSS_ASSISTED_OPTION_KEY: lambda obj, value: Job.SetGnssAssistedOptionFromJobDict(
                obj, value
            ),
            Job.GNSS_ASSISTED_CAPTURE_MODE_KEY: lambda obj, value: Job.SetGnssAssistedCaptureModeFromJobDict(
                obj, value
            ),
            Job.GNSS_ASSISTED_NB_SATELLITE_KEY: lambda obj, value: setattr(
                obj, "gnss_assisted_nb_satellite", value
            ),
            Job.GNSS_ASSISTED_ANTENNA_SELECTION: lambda obj, value: Job.SetGnssAssistedAntennaSelectionFromJobDict(
                obj, value
            ),
            Job.GNSS_ASSISTED_CONSTELLATIONS_KEY: lambda obj, value: Job.SetGnssAssistedConstellationMaskFromJobDict(
                obj, value
            ),
            Job.ASSISTED_COORDINATE_KEY: lambda obj, value: Job.SetCoordinatesFromJobDict(
                obj, value
            ),
        }
        SETTERS[element_key](self, element_value)

    @staticmethod
    def from_dict(job_dict_original):
        job_dict = copy(job_dict_original)

        job_name = job_dict.pop(Job.JOB_NAME_KEY)
        job_n_iteration = job_dict.pop(Job.N_ITERATIONS_KEY)
        job_reset_before_start = job_dict.pop(Job.RESET_BEFORE_JOB_START_KEY)
        job_is_gnss_autonomous = Job.job_dict_is_gnss_autonomous(job_dict)
        job_is_gnss_assisted = Job.job_dict_is_gnss_assisted(job_dict)
        job = Job(
            name=job_name,
            n_iteration=job_n_iteration,
            reset_before_start=job_reset_before_start,
            gnss_autonomous_enable=job_is_gnss_autonomous,
            gnss_assisted_enable=job_is_gnss_assisted,
        )
        for key, val in job_dict.items():
            job.SetJobElement(key, val)
        return job

    def __str__(self):
        str_job_desc = list()
        if self.has_wifi:
            str_job_desc.append(
                "[WiFi: {wifi_chans}, {wifi_types}]".format(
                    wifi_chans=[chan.name for chan in self.wifi_channels],
                    wifi_types=self.wifi_types,
                )
            )
        if self.gnss_autonomous_enable:
            str_job_desc.append("[GNSS Autonomous]")
        if self.gnss_assisted_enable:
            str_job_desc.append("[GNSS Assisted]")
        return "Job {} [{}]".format(self.name, ", ".join(str_job_desc))

    @staticmethod
    def constellations_to_mask(constellations):
        mask = 0
        for constellation in constellations:
            mask |= int.from_bytes(
                GnssConstellation[constellation].value, byteorder="little"
            )
        return mask.to_bytes(length=1, byteorder="little")

    @staticmethod
    def job_dict_is_wifi(job_dict):
        return Job.WIFI_CHANNELS_KEY in job_dict

    @staticmethod
    def job_dict_is_gnss_autonomous(job_dict):
        return Job.GNSS_AUTONOMOUS_OPTION_KEY in job_dict

    @staticmethod
    def job_dict_is_gnss_assisted(job_dict):
        return Job.ASSISTED_COORDINATE_KEY in job_dict

    @property
    def has_gnss_assisted(self):
        return self.gnss_assisted_enable

    @property
    def has_wifi(self):
        return self.wifi_enable_mode is not WifiEnableMode.disabled

    @property
    def has_gnss_autonomous(self):
        return self.gnss_autonomous_enable
