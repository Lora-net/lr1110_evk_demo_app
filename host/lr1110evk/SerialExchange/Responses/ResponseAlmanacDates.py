"""
Define almanac date serial response class

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


class ResponseAlmanacDates(ResponseBase):
    SIZE_SAT_AGE_RAW_BYTES = 3

    def __init__(self, reception_time, almanac_age_per_satellites):
        super().__init__(reception_time)
        self.almanac_age_per_satellites = almanac_age_per_satellites

    def __str__(self):
        return "Almanacs:\n{}".format(
            ";".join(
                [
                    "{}: {}".format(sat, age)
                    for sat, age in self.almanac_age_per_satellites.items()
                ]
            )
        )

    @classmethod
    def get_response_code(cls):
        return b"\x07\x00"

    @classmethod
    def from_response_raw(cls, response_raw):
        reception_time = response_raw.receive_time

        almanac_age_per_satellites = dict()
        for raw_sat_date in ResponseAlmanacDates.chunker(
            response_raw.payload_bytes, ResponseAlmanacDates.SIZE_SAT_AGE_RAW_BYTES
        ):
            satellite_id = raw_sat_date[0]
            age = (raw_sat_date[1] << 8) + raw_sat_date[2]
            almanac_age_per_satellites[satellite_id] = age

        response = ResponseAlmanacDates(
            reception_time=reception_time,
            almanac_age_per_satellites=almanac_age_per_satellites,
        )
        return response

    @staticmethod
    # From https://stackoverflow.com/a/434328
    def chunker(sequence, size):
        for pos in range(0, len(sequence), size):
            yield sequence[pos : pos + size]
