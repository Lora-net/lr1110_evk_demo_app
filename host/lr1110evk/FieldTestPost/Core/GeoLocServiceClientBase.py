"""
Define GeoLoc service client base class

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

from .ResponseBase import (
    ResponseFailureBase,
    ResponseSuccessBase,
    ResponseSuccessReverseGeoLoc,
)
from .Geocoding import GeocodingBuilder
from lr1110evk.BaseTypes import Coordinate
import requests
from json import loads


class GeoLocServiceClientBaseException(Exception):
    pass


class GeoLocServiceTimeoutException(GeoLocServiceClientBaseException):
    def __init__(self, timeout):
        super(GeoLocServiceTimeoutException, self).__init__()
        self.timeout_s = timeout

    def __str__(self):
        return "The geo loc service did not answered after {} second(s)".format(
            self.timeout_s
        )


class GeoLocServiceBadResponseStatus(GeoLocServiceClientBaseException):
    def __init__(self, bad_http_code_text, erroneous_response: ResponseFailureBase):
        self.bad_http_code_text = bad_http_code_text
        self.erroneous_response = erroneous_response

    def __str__(self):
        return (
            "Bad response status from geo loc service: '{}'. Got reason: '{}'".format(
                self.bad_http_code_text, self.erroneous_response.failure_reason
            )
        )


class GeoLocServiceClientBase:
    RESPONSE_TIMEOUT_S = 5
    DEFAULT_BASE_URL = ""
    DEFAULT_PATH_URL = ""
    DEFAULT_PORT = None
    DEFAULT_COMPATIBLE_VERSION_URL = ""

    def __init__(self, server_address, authentication_token):
        self.__server_address = server_address
        self.authentication_token = authentication_token

    @classmethod
    def get_default_base_url(cls):
        return cls.DEFAULT_BASE_URL

    @classmethod
    def get_response_timeout(cls):
        return cls.RESPONSE_TIMEOUT_S

    @classmethod
    def get_default_url_path(cls):
        return cls.DEFAULT_PATH_URL

    @classmethod
    def get_default_url_version(cls):
        return cls.DEFAULT_COMPATIBLE_VERSION_URL

    @classmethod
    def get_default_port_number(cls):
        return cls.DEFAULT_PORT

    @property
    def header(self):
        return self.build_header_from_authentication_token(self.authentication_token)

    @property
    def server_address(self):
        return self.__server_address

    @classmethod
    def build_header_from_authentication_token(cls, authentication_token):
        return {}

    @classmethod
    def from_token_and_url_info(
        cls, authentication_token, baseUrl, port, version, path
    ):
        raise NotImplementedError

    @classmethod
    def from_token_and_default(cls, authentication_token):
        return cls.from_token_and_url_info(
            authentication_token=authentication_token,
            baseUrl=cls.get_default_base_url(),
            port=cls.get_default_port_number(),
            version=cls.get_default_url_version(),
            path=cls.get_default_url_path(),
        )

    def build_error_response(self, http_error_code, response_text):
        erroneous_response = ResponseFailureBase(
            http_code=http_error_code,
            raw_response=response_text,
            failure_reason_from_server=response_text,
        )
        return erroneous_response

    def build_response(self, http_code, response_text):
        raise NotImplementedError

    def produce_response_from_request(self, request_data):
        response = requests.post(
            self.server_address,
            headers=self.build_header_from_authentication_token(
                self.authentication_token
            ),
            data=request_data,
            timeout=self.RESPONSE_TIMEOUT_S,
        )
        return response

    def call_service_and_get_response(self, request_data):
        try:
            response = self.produce_response_from_request(request_data)
        except requests.exceptions.ReadTimeout:
            raise GeoLocServiceTimeoutException(
                GeoLocServiceClientBase.RESPONSE_TIMEOUT_S
            )
        try:
            response.raise_for_status()
        except requests.exceptions.HTTPError as http_error:
            print("Error Code: {}\nResponse:\n{}".format(http_error, response.text))
            erroneous_response = self.build_error_response(http_error, response.text)
            raise GeoLocServiceBadResponseStatus(http_error, erroneous_response)

        return self.build_response(response.status_code, response.text)


class GeoLocServiceClientGnss(GeoLocServiceClientBase):
    DEFAULT_BASE_URL = "https://gls.loracloud.com"
    DEFAULT_PATH_URL = "solve/gnss_lr1110_singleframe"
    DEFAULT_PORT = 443
    DEFAULT_COMPATIBLE_VERSION_URL = "v3"

    @staticmethod
    def build_header_from_authentication_token(authentication_token):
        return {
            "Ocp-Apim-Subscription-Key": authentication_token,
            "Content-Type": "application/json",
        }

    def get_log_message_from_warning(self, response_dict):
        LOG_MESSAGE_KEY = "warnings"
        return response_dict[LOG_MESSAGE_KEY]

    def get_log_message_from_error(self, response_dict):
        ERROR_MESSAGE_KEY = "errors"
        return response_dict[ERROR_MESSAGE_KEY]

    def get_coordinate_accuracy_from_result(self, response_dict):
        RESULT_KEY = "result"
        ACCURACY_KEY = "accuracy"
        REFERENTIAL_KEY = "llh"

        latitude = float(response_dict[RESULT_KEY][REFERENTIAL_KEY][0])
        longitude = float(response_dict[RESULT_KEY][REFERENTIAL_KEY][1])
        altitude = float(response_dict[RESULT_KEY][REFERENTIAL_KEY][2])
        coordinate = Coordinate(
            latitude=latitude, longitude=longitude, altitude=altitude
        )
        accuracy = float(response_dict[RESULT_KEY][ACCURACY_KEY])
        return coordinate, accuracy

    def build_response(self, http_code, response_text):
        response_dict = loads(response_text)

        try:
            coordinates, loc_accuracy = self.get_coordinate_accuracy_from_result(
                response_dict
            )
        except TypeError as err:
            if "NoneType" in str(err):
                failure_text = "errors: {}, warnings: {}".format(
                    self.get_log_message_from_error(response_dict),
                    self.get_log_message_from_warning(response_dict),
                )
                response = ResponseFailureBase(
                    http_code=http_code,
                    raw_response=response_text,
                    failure_reason_from_server=failure_text,
                )
                return response
            raise err
        response = ResponseSuccessBase(
            http_code=http_code,
            raw_response=response_text,
            estimated_coordinate=coordinates,
            accuracy=loc_accuracy,
        )
        return response

    @classmethod
    def from_token_and_url_info(
        cls, authentication_token, baseUrl, port, version, path
    ):
        url = "{base}:{port}/api/{version}/{path}".format(
            base=baseUrl, port=port, path=path, version=version
        )
        return GeoLocServiceClientGnss(url, authentication_token)


class GeoLocServiceClientMultiFrameGnss(GeoLocServiceClientGnss):
    DEFAULT_PATH_URL = "solve/gnss_lr1110_multiframe"

    def get_log_message_from_warning(self, response_dict):
        LOG_MESSAGE_KEY = "warnings"
        return response_dict[LOG_MESSAGE_KEY]

    def get_log_message_from_error(self, response_dict):
        ERROR_MESSAGE_KEY = "errors"
        return response_dict[ERROR_MESSAGE_KEY]

    def build_response(self, http_code, response_text):
        response_dict = loads(response_text)

        try:
            coordinates, loc_accuracy = self.get_coordinate_accuracy_from_result(
                response_dict
            )
        except TypeError as err:
            if "NoneType" in str(err):
                failure_text = "errors: {}, warnings: {}".format(
                    self.get_log_message_from_error(response_dict),
                    self.get_log_message_from_warning(response_dict),
                )
                response = ResponseFailureBase(
                    http_code=http_code,
                    raw_response=response_text,
                    failure_reason_from_server=failure_text,
                )
                return response
            raise err
        except KeyError:
            failure_text = "errors: {}".format(
                self.get_log_message_from_error(response_dict),
            )
            response = ResponseFailureBase(
                http_code=http_code,
                raw_response=response_text,
                failure_reason_from_server=failure_text,
            )
            return response

        response = ResponseSuccessBase(
            http_code=http_code,
            raw_response=response_text,
            estimated_coordinate=coordinates,
            accuracy=loc_accuracy,
        )
        return response

    @classmethod
    def from_token_and_url_info(
        cls, authentication_token, baseUrl, port, version, path
    ):
        url = "{base}:{port}/api/{version}/{path}".format(
            base=baseUrl, port=port, path=path, version=version
        )
        return GeoLocServiceClientMultiFrameGnss(url, authentication_token)


class GeoLocServiceClientWifi(GeoLocServiceClientBase):
    DEFAULT_PATH_URL = "loraWifi"
    DEFAULT_COMPATIBLE_VERSION_URL = "api/v2"
    DEFAULT_BASE_URL = "https://gls.loracloud.com"
    DEFAULT_PORT = 443

    @staticmethod
    def build_header_from_authentication_token(authentication_token):
        return {
            "Ocp-Apim-Subscription-Key": authentication_token,
            "Content-Type": "application/json",
        }

    @classmethod
    def from_token_and_url_info(
        cls, authentication_token, baseUrl, port, version, path
    ):
        url = "{base}:{port}/{version}/{path}".format(
            base=baseUrl, port=port, path=path, version=version
        )
        return GeoLocServiceClientWifi(url, authentication_token)

    def build_response(self, http_code, response_text):
        response_dict = loads(response_text)

        RESULT_KEY = "result"
        LATITUDE_KEY = "latitude"
        LONGITUDE_KEY = "longitude"
        ALTITUDE_KEY = "altitude"
        ACCURACY_KEY = "accuracy"

        coordinates = Coordinate(
            latitude=float(response_dict[RESULT_KEY][LATITUDE_KEY]),
            longitude=float(response_dict[RESULT_KEY][LONGITUDE_KEY]),
            altitude=float(response_dict[RESULT_KEY][ALTITUDE_KEY]),
        )
        loc_accuracy = int(response_dict[RESULT_KEY][ACCURACY_KEY])
        response = ResponseSuccessBase(
            http_code=http_code,
            raw_response=response_text,
            estimated_coordinate=coordinates,
            accuracy=loc_accuracy,
        )
        return response


class GeoLocServiceClientReverseGeoCoding(GeoLocServiceClientBase):
    DEFAULT_PATH_URL = "osm"
    DEFAULT_BASE_URL = "https://osm-geo.loraclouddemo.com"
    DEFAULT_PORT = 443

    def __init__(self, url):
        super().__init__(server_address=url, authentication_token=None)

    @staticmethod
    def build_header_from_authentication_token(authentication_token):
        return dict()

    @classmethod
    def from_url_info(cls, baseUrl, port, path):
        url = "{base}:{port}/{path}".format(base=baseUrl, port=port, path=path)
        return GeoLocServiceClientReverseGeoCoding(url)

    @classmethod
    def from_default(cls):
        return cls.from_url_info(
            baseUrl=cls.get_default_base_url(),
            port=cls.get_default_port_number(),
            path=cls.get_default_url_path(),
        )

    def produce_response_from_request(self, request_data: Coordinate):
        url_request = "{}?latitude={}&longitude={}".format(
            self.server_address, request_data.latitude, request_data.longitude
        )
        response = requests.get(
            url=url_request,
        )
        return response

    def build_response(self, http_code, response_text):
        ADDRESS_KEY = "address"

        response_dict = loads(response_text)

        geo_conding_builder = GeocodingBuilder()
        geo_conding_builder.load_names_aliases()
        reverse_geocoding = geo_conding_builder.build_data(response_dict[ADDRESS_KEY])
        response = ResponseSuccessReverseGeoLoc(
            http_code=http_code,
            raw_response=response_text,
            reverse_geo_loc=reverse_geocoding,
        )
        return response
