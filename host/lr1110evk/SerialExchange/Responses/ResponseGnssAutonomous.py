"""
Define GNSS autonomous serial response class

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

from .ResponseBase import ResponseBase
from lr1110evk.BaseTypes import ScannedGnss


class ResponseGnssAutonomousResult(ResponseBase):
    def __init__(self, receive_time, gnss_scan):
        super().__init__(receive_time)
        self.gnss_scan = gnss_scan

    @classmethod
    def from_response_raw(cls, response_raw):
        receive_time = response_raw.receive_time
        gnss = ScannedGnss.from_bytes(response_raw.payload_bytes, receive_time)
        response_gnss = ResponseGnssAutonomousResult(
            receive_time=receive_time, gnss_scan=gnss
        )
        return response_gnss

    @classmethod
    def get_response_code(cls):
        return b"\x82\x00"

    def __str__(self):
        return "GnssResult({}): {}".format(self.reception_time, self.gnss_scan)
