"""
Define job JSON validator classes

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

from jsl.fields import StringField, ArrayField, BooleanField, NumberField, OneOfField
from jsl import Document, DocumentField
from lr1110evk.BaseTypes.WifiChannels import WifiChannels
from lr1110evk.SerialExchange.Commands.CommandStart import GnssCaptureMode

from jsonschema import validate
from json import load


class WiFiChanelField(StringField):
    def __init__(self, *args, **kwargs):
        super(WiFiChanelField, self).__init__(
            pattern="^{}$".format(
                "|".join([chan.name for chan in WifiChannels.WIFI_CHANNELS])
            ),
            description="The set of Wi-Fi channels to scan.",
            *args,
            **kwargs
        )


class WiFiTypeField(StringField):
    def __init__(self, *args, **kwargs):
        super(WiFiTypeField, self).__init__(
            pattern="^TYPE_(B|G)$", description="Wi-Fi type to scan for."
        )


class WifiApiField(StringField):
    def __init__(self, *args, **kwargs):
        super(WifiApiField, self).__init__(
            pattern="^(wifi_scan|country_code)$",
            description="The Wi-Fi API to use for passive scan operation.",
            *args,
            **kwargs
        )


class WifiModeField(StringField):
    def __init__(self, *args, **kwargs):
        super(WifiModeField, self).__init__(
            pattern="^(beacon_and_packet|beacon_only)$",
            description="The Acquisition Mode to configure for passive scan operations.",
        )


class GnssOptionField(StringField):
    def __init__(self, *args, **kwargs):
        super(GnssOptionField, self).__init__(
            pattern="^(best_effort|default)$", description="Also known as Effort Mode."
        )


class GnssCaptureModeField(StringField):
    def __init__(self, *args, **kwargs):

        super(GnssCaptureModeField, self).__init__(
            pattern="^({})$".format("|".join([mode.name for mode in GnssCaptureMode])),
            description="Select the GNSS scanning strategy.",
        )


class GnssAntennaSelectionField(StringField):
    def __init__(self, *args, **kwargs):
        super(GnssAntennaSelectionField, self).__init__(
            pattern="^(no_selection|select_antenna_1|select_antenna_2)$",
            description="Select a specific antenna before executing the GNSS scan. This option if used requires a dedicated hardware.",
        )


class GnssConstellationField(StringField):
    def __init__(self, *args, **kwargs):
        super(GnssConstellationField, self).__init__(
            pattern="^(gps|beidou)$",
            description="Indicates which constellation to be scanned for during scanning operation.",
        )


class CommonJobDocument(Document):
    name = StringField(description="Name of the job.")
    n_iterations = NumberField(
        minimum=0,
        multiple=1,
        description="Configure the number of time the job should be repeated before executing the next job.",
    )
    reset_before_job_start = BooleanField(
        description="Configures the reset of LR1110 strategy for this job. If set to true, the LR1110 is reset before executing this job. If set to false, it is not reset before executing this job."
    )


class WifiJobDocument(CommonJobDocument):
    wifi_api = WifiApiField(required=True)
    wifi_channels = ArrayField(WiFiChanelField())
    wifi_types = ArrayField(WiFiTypeField())
    wifi_nbr_retrial = NumberField(
        minimum=0,
        multiple=1,
        description="Sets the number of Wi-Fi passive scan attempts to try per channels.",
    )
    wifi_max_result_per_scan = NumberField(
        minimum=0,
        multiple=1,
        description="Indicates the maximal number of Wi-Fi MAC addresses to fetch during passive scan operation.",
    )
    wifi_timeout = NumberField(
        minimum=0,
        multiple=1,
        description="Configures the maximal wait duration for a Wi-Fi preamble before considering a Wi-Fi channel is empty of signal. Expressed in ms.",
    )
    wifi_abort_on_timeout = BooleanField(
        description="Enable or disable the Wi-Fi abort on timeout. If set to True, the Wi-Fi scan will jump to next channel as soon as a preamble timeout is reached.",
        required=True,
    )
    wifi_mode = WifiModeField()


class AssistedCoordinateDocument(Document):
    latitude = NumberField(
        description="Latitude to provide to the GNSS scan API. Expressed in decimal degree."
    )
    longitude = NumberField(
        description="Longitude to provide to the GNSS scan API. Expressed in decimal degree."
    )
    altitude = NumberField(description="Altitude to provide to the GNSS scan API.")


class CommonGnssJobDocument(CommonJobDocument):
    n_scan_iteration = NumberField(
        multiple_of=1,
        minimum=1,
        description="Number of scans to execute per iteration. Results of all scans in a single iteration are aggregated.",
        required=True,
    )


class GnssAutonomousDocument(CommonGnssJobDocument):
    gnss_autonomous_option = GnssOptionField()
    gnss_autonomous_capture_mode = GnssCaptureModeField()
    gnss_autonomous_nb_satellite = NumberField(
        multiple_of=1,
        minimum=0,
        maximum=255,
        description="Number of satellites to fetch during GNSS scan operation.",
    )
    gnss_autonomous_antenna_selection = GnssAntennaSelectionField()
    gnss_autonomous_constellations = ArrayField(GnssConstellationField())


class GnssAssistedDocument(CommonGnssJobDocument):
    gnss_assisted_option = GnssOptionField()
    gnss_assisted_capture_mode = GnssCaptureModeField()
    gnss_assisted_nb_satellite = NumberField(
        multiple_of=1,
        minimum=0,
        maximum=255,
        description="Number of satellites to fetch during GNSS scan operation.",
    )
    gnss_assisted_antenna_selection = GnssAntennaSelectionField()
    gnss_assisted_constellations = ArrayField(GnssConstellationField())
    assisted_coordinate = DocumentField(AssistedCoordinateDocument)


class JobsDocument(Document):
    infinite_loops = BooleanField(
        description="Configures the repetition of the jobs. If set to true, the job file will be repeated infinitely. If set to false, the field test will end at the end of the last job in this file."
    )
    scan_interval = NumberField(
        minimum=0,
        multiple_of=1,
        description="Number of seconds to wait between two consecutive list of jobs execution. Useful only if infinite_loops is true. Value in seconds. 0 means that there is no wait between list of jobs.",
        required=True,
    )
    jobs = ArrayField(
        OneOfField(
            fields=[
                DocumentField(WifiJobDocument),
                DocumentField(GnssAutonomousDocument),
                DocumentField(GnssAssistedDocument),
            ]
        ),
        description="Array of jobs description to be executed during field tests.",
    )


class JobValidator:
    _schema = JobsDocument.get_schema()

    def __init__(self):
        pass

    @staticmethod
    def validate_job_json_file(json_file):
        with open(json_file, "r") as fp:
            json_stream = load(fp)
        JobValidator.validate_job_json_stream(json_stream)

    @staticmethod
    def validate_job_json_stream(json_stream):
        validate(schema=JobValidator._schema, instance=json_stream)

    @staticmethod
    def get_schema():
        return JobValidator._schema
