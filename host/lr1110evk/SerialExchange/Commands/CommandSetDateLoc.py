"""
Define GNSS date and location configuration serial command class

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

from .CommandBase import CommandBase


class CommandSetDateLoc(CommandBase):
    LOC_MULTIPLIER = 1000

    def __init__(self):
        self._gps_time = None
        self._gps_assisted_location = None

    @staticmethod
    def get_com_code():
        return b"\x05\x00"

    def payload_to_bytes(self):
        gnss_seconds = self.gps_time.to_bytes(4, byteorder="little")
        longitude, latitude, altitude = (
            int(element * CommandSetDateLoc.LOC_MULTIPLIER).to_bytes(
                4, byteorder="little", signed=True
            )
            for element in [
                self._gps_assisted_location.longitude,
                self._gps_assisted_location.latitude,
                self._gps_assisted_location.altitude,
            ]
        )
        return gnss_seconds + longitude + latitude + altitude

    @property
    def gps_time(self):
        return self._gps_time

    @gps_time.setter
    def gps_time(self, gnss_time):
        self._gps_time = gnss_time

    @property
    def gps_assisted_location(self):
        return self._gps_assisted_location

    @gps_assisted_location.setter
    def gps_assisted_location(self, location):
        self._gps_assisted_location = location
