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

from .ResponseBase import ResponseFailureBase, ResponseSuccessBase, ResponseSuccessReverseGeoLoc
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
        return "Bad response status from geo loc service: '{}'. Got reason: '{}'".format(
            self.bad_http_code_text, self.erroneous_response.failure_reason
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
    def from_token_and_url_info(cls, authentication_token, baseUrl, port, version, path):
        raise NotImplementedError

    @classmethod
    def from_token_and_default(cls, authentication_token):
        return cls.from_token_and_url_info(
            authentication_token=authentication_token,
            baseUrl=cls.get_default_base_url(),
            port=cls.get_default_port_number(),
            version=cls.get_default_url_version(),
            path=cls.get_default_url_path()
        )

    def build_error_response(self, http_error_code, response_text):
        erroneous_response = ResponseFailureBase(
            http_code=http_error_code,
            raw_response=response_text,
            failure_reason_from_server=response_text
        )
        return erroneous_response

    def build_response(self, http_code, response_text):
        raise NotImplementedError

    def produce_response_from_request(self, request_data):
        response = requests.post(
            self.server_address,
            headers=self.build_header_from_authentication_token(self.authentication_token),
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
    DEFAULT_BASE_URL = "https://das.loracloud.com"
    DEFAULT_PATH_URL = "uplink/send"
    DEFAULT_PORT = 443
    DEFAULT_COMPATIBLE_VERSION_URL = "v1"

    @staticmethod
    def build_header_from_authentication_token(authentication_token):
        return {"Authorization": authentication_token, "Content-Type": "application/json"}

    def get_first_result(self, response_dict):
        RESULT_KEY = "result"
        # A result structure can have results for several devices. In the frame of the application
        # we assume that there is only one result which is the one we want
        all_results_dict = response_dict[RESULT_KEY]
        first_result_key = list(all_results_dict.keys())[0]
        result_first_device = all_results_dict[first_result_key][RESULT_KEY]
        return result_first_device

    def get_log_message_from_result(self, result_device):
        LOG_MESSAGE_KEY = "log_messages"
        LOG_MESSAGE_KEY_AGAIN = "logmsg"
        return result_device[LOG_MESSAGE_KEY][0][LOG_MESSAGE_KEY_AGAIN]

    def get_log_message_from_error(self, response_dict):
        RESULT_KEY = "result"
        ERROR_MESSAGE_KEY = "error"
        # A result structure can have results for several devices. In the frame of the application
        # we assume that there is only one result which is the one we want
        all_results_dict = response_dict[RESULT_KEY]
        first_error_key = list(all_results_dict.keys())[0]
        error_first_device = all_results_dict[first_error_key][ERROR_MESSAGE_KEY]
        print(error_first_device)
        return error_first_device

    def get_coordinate_accuracy_from_result(self, result_device):
        POSITION_SOLUTION_KEY = "position_solution"
        ACCURACY_KEY = "accuracy"
        REFERENTIAL_KEY = "llh"

        latitude = float(result_device[POSITION_SOLUTION_KEY][REFERENTIAL_KEY][0])
        longitude = float(result_device[POSITION_SOLUTION_KEY][REFERENTIAL_KEY][1])
        altitude = float(result_device[POSITION_SOLUTION_KEY][REFERENTIAL_KEY][2])
        coordinate = Coordinate(
            latitude=latitude,
            longitude=longitude,
            altitude=altitude
        )
        accuracy = float(result_device[POSITION_SOLUTION_KEY][ACCURACY_KEY])
        return coordinate, accuracy

    def build_response(self, http_code, response_text):
        response_dict = loads(response_text)
        from pprint import pprint
        pprint(response_dict)
        try:
            result_first_device = self.get_first_result(response_dict)
        except TypeError as err:
            if "NoneType" in str(err):
                # It is due to the field "position_solution" set to null
                failure_text = self.get_log_message_from_result(result_first_device)
                response = ResponseFailureBase(
                    http_code=http_code,
                    raw_response=response_text,
                    failure_reason_from_server=failure_text
                )
                return response
            raise err
        except KeyError:
            # It is due to the inexistance of the field result
            failure_text = self.get_log_message_from_error(response_dict)
            response = ResponseFailureBase(
                http_code=http_code,
                raw_response=response_text,
                failure_reason_from_server=failure_text
            )
            return response
        try:
            coordinates, loc_accuracy = self.get_coordinate_accuracy_from_result(result_first_device)
        except TypeError as err:
            if "NoneType" in str(err):
                failure_text = self.get_log_message_from_result(result_first_device)
                response = ResponseFailureBase(
                    http_code=http_code,
                    raw_response=response_text,
                    failure_reason_from_server=failure_text
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
    def from_token_and_url_info(cls, authentication_token, baseUrl, port, version, path):
        url = "{base}:{port}/api/{version}/{path}".format(
            base=baseUrl, port=port, path=path, version=version
        )
        return GeoLocServiceClientGnss(url, authentication_token)


class GeoLocServiceClientWifi(GeoLocServiceClientBase):
    DEFAULT_PATH_URL = "loraWifi"
    DEFAULT_COMPATIBLE_VERSION_URL = "api/v2"
    DEFAULT_BASE_URL = "https://gls.loracloud.com"
    DEFAULT_PORT = 443

    @staticmethod
    def build_header_from_authentication_token(authentication_token):
        return {"Ocp-Apim-Subscription-Key": authentication_token, "Content-Type": "application/json"}

    @classmethod
    def from_token_and_url_info(cls, authentication_token, baseUrl, port, version, path):
        url = "{base}:{port}/{version}/{path}".format(
            base=baseUrl, port=port, path=path, version=version
        )
        return GeoLocServiceClientWifi(url, authentication_token)

    def build_response(self, http_code, response_text):
        response_dict = loads(response_text)
        from pprint import pprint
        pprint(response_dict)

        RESULT_KEY = "result"
        LATITUDE_KEY = "latitude"
        LONGITUDE_KEY = "longitude"
        ALTITUDE_KEY = "altitude"
        ACCURACY_KEY = "accuracy"

        coordinates = Coordinate(
            latitude=float(
                response_dict[RESULT_KEY][LATITUDE_KEY]
            ),
            longitude=float(
                response_dict[RESULT_KEY][LONGITUDE_KEY]
            ),
            altitude=float(
                response_dict[RESULT_KEY][ALTITUDE_KEY]
            ),
        )
        loc_accuracy = int(response_dict[RESULT_KEY][ACCURACY_KEY])
        response = ResponseSuccessBase(http_code=http_code, raw_response=response_text, estimated_coordinate=coordinates, accuracy=loc_accuracy)
        return response


class GeoLocServiceClientReverseGeoCoding(GeoLocServiceClientBase):
    DEFAULT_PATH_URL = "osm"
    DEFAULT_BASE_URL = "https://osm-geo.loraclouddemo.com"
    DEFAULT_PORT = 443

    def __init__(self, url):
        super().__init__(
            server_address=url,
            authentication_token=None
        )

    @staticmethod
    def build_header_from_authentication_token(authentication_token):
        return dict()

    @classmethod
    def from_url_info(cls, baseUrl, port, path):
        url = "{base}:{port}/{path}".format(
            base=baseUrl, port=port, path=path
        )
        return GeoLocServiceClientReverseGeoCoding(url)

    @classmethod
    def from_default(cls):
        return cls.from_url_info(
            baseUrl=cls.get_default_base_url(),
            port=cls.get_default_port_number(),
            path=cls.get_default_url_path(),
        )

    def produce_response_from_request(self, request_data: Coordinate):
        url_request = f"{self.server_address}?latitude={request_data.latitude}&longitude={request_data.longitude}"
        response = requests.get(
            url=url_request,
        )
        return response

    def build_response(self, http_code, response_text):
        ADDRESS_KEY = "address"

        response_dict = loads(response_text)
        from pprint import pprint
        pprint(response_dict)

        geo_conding_builder = GeocodingBuilder()
        geo_conding_builder.load_names_aliases()
        reverse_geocoding = geo_conding_builder.build_data(response_dict[ADDRESS_KEY])
        response = ResponseSuccessReverseGeoLoc(http_code=http_code, raw_response=response_text, reverse_geo_loc=reverse_geocoding)
        return response
