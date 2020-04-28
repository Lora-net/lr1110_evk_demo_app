"""
Define get version serial response class

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


class ResponseVersion(ResponseBase):
    def __init__(
        self,
        reception_time,
        software_version,
        driver_version,
        lr1110_hw_version,
        lr1110_type,
        lr1110_firmware_version,
        almanac_crc,
        chip_uid,
    ):
        super().__init__(reception_time)
        self.software_version = software_version
        self.driver_version = driver_version
        self.lr1110_hw_version = lr1110_hw_version
        self.lr1110_type = lr1110_type
        self.lr1110_firmware_version = lr1110_firmware_version
        self.almanac_crc = almanac_crc
        self.chip_uid = chip_uid

    def __str__(self):
        return "Version: software: {}, LR1110 HW: {}, LR1110 FW: {}, almanac crc: {}, chip uid: {}".format(
            self.software_version,
            self.lr1110_hw_version,
            self.lr1110_firmware_version,
            self.almanac_crc,
            self.chip_uid,
        )

    @classmethod
    def get_response_code(cls):
        return b"\x06\x00"

    @classmethod
    def from_response_raw(cls, response_raw):
        reception_time = response_raw.receive_time
        (
            software_version,
            driver_version_raw,
            lr1110_hw_version_raw,
            lr1110_type_raw,
            lr1110_firmware_version_raw,
            almanac_crc_raw,
            chip_uid,
        ) = response_raw.payload_bytes.split(b";")
        response = ResponseVersion(
            reception_time=reception_time,
            software_version=software_version.decode(),
            driver_version=driver_version_raw.decode(),
            lr1110_hw_version=lr1110_hw_version_raw.decode(),
            lr1110_type=lr1110_type_raw.decode(),
            lr1110_firmware_version=lr1110_firmware_version_raw.decode(),
            almanac_crc=almanac_crc_raw.decode(),
            chip_uid=chip_uid.decode()
        )
        return response
