"""
Define Version class

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

from pkg_resources import get_distribution


class VersionException(Exception):
    pass


class MalformedVersionStringException(VersionException):
    def __init__(self, malformed_string):
        self.malformed_string = malformed_string

    def __str__(self):
        return "Cannot build version information from string '{}'".format(
            self.malformed_string
        )


class Version:
    VERSION_TOKEN = "VERSION"

    def __init__(
        self,
        host_version,
        demo_version,
        driver_version,
        lr1110_version,
        almanac_crc,
        almanac_ages,
        chip_uid
    ):
        self.host_version = host_version
        self.demo_version = demo_version
        self.driver_version = driver_version
        self.lr1110_version = lr1110_version
        self.almanac_crc = almanac_crc
        self.almanac_ages = almanac_ages
        self.chip_uid = chip_uid

    def __str__(self):
        return "Host: {}, Demo: {}, Driver: {}, LR1110: {}, chip uid: {}".format(
            self.host_version,
            self.demo_version,
            self.driver_version,
            self.lr1110_version,
            self.chip_uid,
        )

    @staticmethod
    def get_host_version():
        return get_distribution("lr1110evk").version

    def to_metadata(self):
        return "{}:{}:{}:{}:{}:{}:{}".format(
            self.host_version,
            self.demo_version,
            self.driver_version,
            self.lr1110_version,
            self.almanac_crc,
            Version.almanac_ages_to_string(self.almanac_ages),
            self.chip_uid,
        )

    @staticmethod
    def almanac_ages_to_string(almanac_ages_dict):
        return "|".join(
            ["{},{}".format(sat, age) for sat, age in almanac_ages_dict.items()]
        )

    @staticmethod
    def almanac_ages_from_string(almanac_ages_str):
        list_almanac_age_raw = almanac_ages_str.split("|")
        age_per_sat = dict()
        for almanac_age_raw in list_almanac_age_raw:
            sat_raw, age_raw = almanac_age_raw.split(",")
            sat = int(sat_raw)
            age = int(age_raw)
            age_per_sat[sat] = age
        return age_per_sat

    @staticmethod
    def from_metadata(metadata):
        (
            host_version,
            demo_version,
            driver_version,
            lr1110_version,
            almanac_crc,
            almanac_ages_str,
            chip_uid,
        ) = metadata.split(":")
        almanac_ages = Version.almanac_ages_from_string(almanac_ages_str)
        return Version(
            host_version=host_version,
            demo_version=demo_version,
            driver_version=driver_version,
            lr1110_version=lr1110_version,
            almanac_crc=almanac_crc,
            almanac_ages=almanac_ages,
            chip_uid=chip_uid.strip()
        )

    @staticmethod
    def from_string(str_version):
        host_version = get_distribution("lr1110evk").version
        try:
            (
                _,
                demo_version,
                driver_version,
                lr1110_version,
                almanac_crc,
                chip_uid,
            ) = str_version.split(":")
        except:
            raise MalformedVersionStringException(str_version)
        return Version(
            host_version,
            demo_version,
            driver_version,
            lr1110_version,
            almanac_crc,
            dict(),
            chip_uid
        )
