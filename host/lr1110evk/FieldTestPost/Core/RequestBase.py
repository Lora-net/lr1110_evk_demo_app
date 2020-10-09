"""
Define request base builder class

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

from json import dumps
from lr1110evk.BaseTypes import ScannedMacAddress
from datetime import timezone


class RequestBase:
    def _to_json_dict(self):
        raise NotImplementedError

    def to_json(self):
        return dumps(self._to_json_dict())

    def __str__(self):
        return self.to_json()


class RequestGnssGls(RequestBase):
    def __init__(self, payload, timestamp, aiding_coordinate):
        self.__payload = payload
        self.__timestamp = timestamp
        self.__aiding_coordinate = aiding_coordinate

    @property
    def payload(self):
        return self.__payload

    @property
    def timestamp(self):
        return self.__timestamp

    @property
    def aiding_coordinate(self):
        return self.__aiding_coordinate

    def _to_json_dict(self):
        json_dict = {
            "payload": self.payload[
                2:
            ],  # The very first byte (destinationID) is removed here as it should not be sent to the solver
            "gnss_capture_time": self.timestamp.replace(tzinfo=timezone.utc).timestamp()
            + 18
            - 315964800,
            # "capture_time_accuracy": 120,
        }
        if self.aiding_coordinate:
            json_dict["gnss_assist_position"] = [
                self.aiding_coordinate.latitude,
                self.aiding_coordinate.longitude,
            ]

        return json_dict


class RequestWifiGls(RequestBase):
    def __init__(self):
        self.macs = list()

    def _to_json_dict(self):
        return {
            "lorawan": [
                {
                    "gatewayId": "00",
                    "rssi": 0,
                    "snr": 0,
                    "toa": 0,
                    "antennaId": 0,
                    "antennaLocation": {"latitude": 0, "longitude": 0, "altitude": 0,},
                },
            ],
            "wifiAccessPoints": [
                RequestWifiGls.mac_to_json_dict(mac) for mac in self.macs
            ],
        }

    @staticmethod
    def mac_to_json_dict(mac: ScannedMacAddress):
        return {
            "macAddress": mac.mac_address,
            "signalStrength": mac.rssi,
        }
