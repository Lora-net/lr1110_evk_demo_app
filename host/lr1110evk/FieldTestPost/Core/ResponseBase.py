"""
Define response parser base class

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

from json import loads, JSONDecodeError
from lr1110evk.BaseTypes import Coordinate
from .Geocoding import GeocodingBuilder


class ResponseBaseException(Exception):
    pass


class ResponseStatusUnknownException(ResponseBaseException):
    def __init__(self, status_str):
        self.status_str = status_str

    def __str__(self):
        return "Unknown status value: '{}'".format(self.status_str)


class ResponseNoCoordinateException(ResponseBaseException):
    def __init__(self, status_str):
        self.status_str = status_str

    def __str__(self):
        return "No coordinates available from failed response. Status: '{}'".format(
            self.status_str
        )


class ResponseMalformedException(ResponseBaseException):
    def __str__(self):
        return "Malformed response from server"


class ResponseStatus:
    _is_error = None

    def __init__(self, http_code, reason):
        self.__reason = reason
        self.__http_code = http_code

    @property
    def isError(self):
        return self._is_error

    @property
    def http_code(self):
        return self.__http_code

    @property
    def reason(self):
        return self.__reason


class ResponseStatusOk(ResponseStatus):
    _is_error = False

    def __init__(self, http_code):
        super().__init__(http_code=http_code, reason="Success")

    def __str__(self):
        return "ok ({})".format(self.http_code)


class ResponseStatusNok(ResponseStatus):
    _is_error = True

    def __init__(self, http_code, reason):
        if reason:
            super().__init__(http_code=http_code, reason=reason)
        else:
            super().__init__(http_code=http_code, reason="No reason from server")

    def __str__(self):
        return "nok ({}): '{}'".format(self.http_code, self.reason)


class ResponseBase:
    def __init__(self, status, raw_response):
        self.__status = status
        self.__raw_response = raw_response

    @property
    def status(self):
        return self.__status

    @property
    def raw_response(self):
        return self.__raw_response

    @property
    def http_code(self):
        return self.status.http_code

    @property
    def is_ok(self):
        return not self.status.isError


class ResponseSuccessBase(ResponseBase):
    def __init__(self, http_code, raw_response, estimated_coordinate, accuracy):
        super().__init__(
            status=ResponseStatusOk(http_code=http_code), raw_response=raw_response
        )
        self.__estimated_coordinate = estimated_coordinate
        self.__accuracy = accuracy

    @property
    def estimated_coordinates(self):
        return self.__estimated_coordinate

    @property
    def loc_accuracy(self):
        return self.__accuracy


class ResponseFailureBase(ResponseBase):
    def __init__(self, http_code, raw_response, failure_reason_from_server):
        super().__init__(
            status=ResponseStatusNok(
                http_code=http_code, reason=failure_reason_from_server
            ),
            raw_response=raw_response,
        )

    @property
    def failure_reason(self):
        return self.status.reason

    @property
    def estimated_coordinates(self):
        raise ResponseNoCoordinateException(self.failure_reason)

    def __str__(self):
        return f"{self.failure_reason}"


class ResponseSuccessReverseGeoLoc(ResponseBase):
    def __init__(self, http_code, raw_response, reverse_geo_loc):
        super().__init__(
            status=ResponseStatusOk(http_code=http_code), raw_response=raw_response
        )
        self.__reverse_geo_loc = reverse_geo_loc

    @property
    def reverse_geo_loc(self):
        return self.__reverse_geo_loc
