"""
Define Almanac update strategy class

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

from ..SerialExchange import (
    CommandUpdateAlmanac,
    CommandCheckAlmanacUpdate,
    CommunicationHandler,
)
from ..SerialExchange import ResponseUpdateAlmanac


class UpdateAlmanacException(Exception):
    pass


class UpdateAlmanacWrongResponseException(UpdateAlmanacException):
    def __init__(self, response_received):
        self.response_received = response_received

    def __str__(self):
        return "Received unexpected response while updating almanac: {}".format(
            self.response_received
        )


class UpdateAlmanacDownloadFailure(UpdateAlmanacException):
    def __str__(self):
        return "Almanac Update failure reported while downloading the almanacs"


class UpdateAlmanacCheckFailure(UpdateAlmanacException):
    def __str__(self):
        return (
            "Almanac Update failure reported when checking the almanac after download"
        )


class UpdateAlmanacJob:
    def __init__(
        self,
        communication_handler: CommunicationHandler,
        almanac_bytestream: bytes,
        expected_crc: bytes,
        logger=None,
    ):
        self.almanac_bytestream = almanac_bytestream
        self.expected_crc = expected_crc
        self.communication_handler = communication_handler
        self.logger = logger

    def log(self, info):
        if self.logger:
            self.logger.log(info)

    def execute_update(self):
        self.push_bytestream()
        self.check_update()

    def push_bytestream(self):
        def command_update_generator(bytestream: bytes):
            size_burst = 20
            for index in range(0, len(bytestream), size_burst):
                burst_bytestream = bytestream[index : index + size_burst]
                command_update_alamac = CommandUpdateAlmanac(burst_bytestream)
                yield command_update_alamac

        # Start by sending all the update commands
        self.log("Start downloading to embedded...")
        for command in command_update_generator(self.almanac_bytestream):
            (
                command_sent,
                response_received,
            ) = self.communication_handler.handle_exchange(command)
            if not self.is_exchange_valid(command_sent, response_received):
                raise UpdateAlmanacWrongResponseException(response_received)
            # From this point, the response_received is a ResponseUpdateAlmanac
            if not response_received.ack_status:
                raise UpdateAlmanacDownloadFailure()
            self.log(".")
        self.log("Downloading terminated")

    def check_update(self):
        self.log("Checking...")
        # Send the check commands
        check_command = CommandCheckAlmanacUpdate(self.expected_crc)
        command_sent, response_received = self.communication_handler.handle_exchange(
            check_command
        )
        if not self.is_exchange_valid(command_sent, response_received):
            raise UpdateAlmanacWrongResponseException(response_received)
        if not response_received.ack_status:
            raise UpdateAlmanacCheckFailure()
        self.log("Check terminated")

    @staticmethod
    def is_exchange_valid(command, response):
        return command.get_com_code() == response.get_response_code()
