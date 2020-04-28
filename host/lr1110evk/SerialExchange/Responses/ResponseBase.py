"""
Define serial response base class

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


class ResponseBaseException(Exception):
    pass


class ResponseMalformedException(ResponseBaseException):
    def __init__(self, response_raw):
        self.malformed_response = response_raw

    def __str__(self):
        return "Malformed response: {}".format(self.malformed_response)


class ResponseRaw:
    def __init__(self, resp_code, payload, receive_time):
        self.resp_code = resp_code
        self.payload_bytes = payload
        self.receive_time = receive_time

    def __str__(self):
        return "[{}] - {}: {}".format(
            self.receive_time, self.resp_code, self.payload_bytes
        )

    def __repr__(self):
        return "ResponseBase<{}, {}, {}>".format(
            self.resp_code, self.payload_bytes, self.receive_time
        )


class ResponseBase:
    def __init__(self, reception_time):
        self.reception_time = reception_time

    @classmethod
    def check_response_code(cls, raw_response):
        return raw_response.resp_code == cls.get_response_code()

    @classmethod
    def get_response_code(cls):
        raise NotImplementedError

    @classmethod
    def from_response_raw(cls, response_raw):
        raise NotImplementedError


class ResponseAck(ResponseBase):
    def __init__(self, receive_time, ack_status):
        super().__init__(receive_time)
        self.ack_status = ack_status

    def __str__(self):
        return "{} ({}): {}".format(
            self.__class__.__name__,
            self.get_response_code(),
            "Ok" if self.ack_status else "Not Ok",
        )

    @classmethod
    def from_response_raw(cls, response_raw):
        ack_value = int.from_bytes(response_raw.payload_bytes[0:1], byteorder="little")
        if ack_value == 0:
            ack = False
        elif ack_value == 1:
            ack = True
        else:
            raise ResponseMalformedException(response_raw)
        response = cls(receive_time=response_raw.receive_time, ack_status=ack)
        return response
