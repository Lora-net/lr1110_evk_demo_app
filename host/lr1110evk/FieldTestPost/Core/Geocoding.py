"""
Define Geocoding builder class

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

from yaml import load_all
from pkg_resources import resource_string
from unicodedata import normalize


class GeocodingBuilder:
    HOST_ASSET_DIR = "assets"
    GEOCODING_COMPONENT_FILENAME = "geocoding_components.yaml"

    ROAD_KEY_NAME = "road"
    CITY_KEY_NAME = "city"
    COUNTRY_KEY_NAME = "country"
    COUNTRY_CODE_KEY_NAME = "country_code"
    STATE_KEY_NAME = "state"
    POSTCODE_KEY_NAME = "postcode"
    HOUSE_KEY_NAME = "house"

    def __init__(self):
        self.geo_coding_file = GeocodingBuilder.get_resource_path()
        self.name_aliases = None

    def load_names_aliases(self):
        self.name_aliases = [doc for doc in load_all(self.geo_coding_file)]

    def build_data(self, osm_json):
        road = GeocodingBuilder.extract_from_name_or_none(
            self.name_aliases, GeocodingBuilder.ROAD_KEY_NAME, osm_json
        )
        city = GeocodingBuilder.extract_from_name_or_none(
            self.name_aliases, GeocodingBuilder.CITY_KEY_NAME, osm_json
        )
        country = GeocodingBuilder.extract_from_name_or_none(
            self.name_aliases, GeocodingBuilder.COUNTRY_KEY_NAME, osm_json
        )
        country_code = GeocodingBuilder.extract_from_name_or_none(
            self.name_aliases, GeocodingBuilder.COUNTRY_CODE_KEY_NAME, osm_json
        )
        state = GeocodingBuilder.extract_from_name_or_none(
            self.name_aliases, GeocodingBuilder.STATE_KEY_NAME, osm_json
        )
        postcode = GeocodingBuilder.extract_from_name_or_none(
            self.name_aliases, GeocodingBuilder.POSTCODE_KEY_NAME, osm_json
        )
        house = GeocodingBuilder.extract_from_name_or_none(
            self.name_aliases, GeocodingBuilder.HOUSE_KEY_NAME, osm_json
        )
        data = GeoCodingData(
            GeocodingBuilder.remove_accents(road),
            GeocodingBuilder.remove_accents(city),
            GeocodingBuilder.remove_accents(country),
            GeocodingBuilder.remove_accents(country_code),
            GeocodingBuilder.remove_accents(state),
            GeocodingBuilder.remove_accents(postcode),
            GeocodingBuilder.remove_accents(house),
        )
        return data

    @staticmethod
    def remove_accents(message: str):
        if message:
            return normalize("NFD", message).encode("ASCII", "ignore").decode("utf-8")
        else:
            return message

    @staticmethod
    def extract_from_name_or_none(name_aliases, name, osm_dict):
        if name_aliases:
            try:
                aliases = [al["aliases"] for al in name_aliases if al["name"] == name][
                    0
                ]
            except KeyError:
                aliases = list()
            keys = [name] + aliases
        else:
            keys = [name]
        for key in keys:
            try:
                val = osm_dict[key]
            except KeyError:
                val = None
            else:
                break
        return val

    @staticmethod
    def get_resource_path():
        resource_package = "lr1110evk"
        resource_geo_coding_file_package_path = "/".join(
            (
                GeocodingBuilder.HOST_ASSET_DIR,
                GeocodingBuilder.GEOCODING_COMPONENT_FILENAME,
            )
        )
        file_path = resource_string(
            resource_package, resource_geo_coding_file_package_path
        )
        return file_path


class GeoCodingData:
    def __init__(self, street, town, country, country_code, state, postcode, house):
        self._street = street
        self._town = town
        self._country = country
        self._country_code = country_code.upper()
        self._state = state
        self._postcode = postcode
        self._house = house

    def __str__(self):
        return "{item._street},{item._town},{item._country_code}".format(item=self)

    @property
    def street(self):
        return self._street

    @property
    def town(self):
        return self._town

    @property
    def country(self):
        return self._country

    @property
    def country_code(self):
        return self._country_code

    @property
    def state(self):
        return self._state

    @property
    def postcode(self):
        return self._postcode

    @property
    def house(self):
        return self._house
