"""
Define serial communication handler class

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

from queue import Empty
from .Responses import (
    ResponseStatus,
    ResponseStartAck,
    ResponseConfigureAck,
    ResponseFetchResult,
    ResponseWifiResult,
    ResponseGnssAutonomousResult,
    ResponseGnssAssistedResult,
    ResponseReset,
    ResponseSetDateLoc,
    ResponseLog,
    ResponseEvent,
    ResponseVersion,
    ResponseAlmanacDates,
    ResponseUpdateAlmanac,
    ResponseCheckAlmanacUpdate,
)


class CommunicationHandlerException(Exception):
    pass


class CommunicationHandlerNoResponse(CommunicationHandlerException):
    def __init__(self, timeout_s):
        self.timeout_s = timeout_s

    def __str__(self):
        return "Was expecting a response but nothing where received in {} second(s)".format(
            self.timeout_s
        )


class CommunicationHandlerUnknownResponseException(CommunicationHandlerException):
    def __init__(self, unknown_raw_command):
        self.unknown_raw_command = unknown_raw_command

    def __str__(self):
        return "Unknown command: {}".format(self.unknown_raw_command)


class CommunicationHandlerSerialNotListeningException(CommunicationHandlerException):
    def __str__(self):
        return "Serial Handler should be started but now it is stopped"


class CommunicationHandler:
    RESPONSE_TIMEOUT = 1
    KNOWN_RESPONSES = [
        ResponseStatus,
        ResponseStartAck,
        ResponseConfigureAck,
        ResponseFetchResult,
        ResponseWifiResult,
        ResponseGnssAutonomousResult,
        ResponseGnssAssistedResult,
        ResponseReset,
        ResponseSetDateLoc,
        ResponseLog,
        ResponseEvent,
        ResponseVersion,
        ResponseAlmanacDates,
        ResponseUpdateAlmanac,
        ResponseCheckAlmanacUpdate,
    ]

    def __init__(self, serial_handler, logger):
        self.serial_handler = serial_handler
        self.logger = logger
        self.RESP_CODE_HANDLER_MAPPER = {
            resp.get_response_code(): resp.from_response_raw
            for resp in CommunicationHandler.KNOWN_RESPONSES
        }

    def log(self, message, date=None):
        self.logger.log(message, date)

    def start(self):
        self.serial_handler.start_read()

    def stop(self):
        self.serial_handler.stop_read()
        self.serial_handler.close()

    def wait_embedded_to_be_configured_for_field_test(self, timeout=None):
        self.serial_handler.is_embedded_set_to_field_test.wait(timeout=timeout)

    def handle_exchange(self, command):
        command = self.send_one_command(command)
        response = self.wait_and_handle_response()
        return command, response

    def send_one_command(self, command):
        command_bytes = command.convert_to_bytes()
        command.sent_time = self.serial_handler.send(command_bytes)
        return command

    def has_received_event(self):
        try:
            response = self.get_response_from_fifo_and_filter_out_log(
                timeout=CommunicationHandler.RESPONSE_TIMEOUT
            )
        except Empty:
            return False
        if response.get_response_code() == ResponseEvent.get_response_code():
            return True
        else:
            return False

    def get_response_from_fifo_and_filter_out_log(self, timeout):
        while True:
            if not self.serial_handler.read_thread.is_alive():
                raise CommunicationHandlerSerialNotListeningException()
            response_raw = self.serial_handler.response_fifo.get(timeout=timeout)
            response = self.handle_response(response_raw)
            if response.get_response_code() == ResponseLog.get_response_code():
                self.log(
                    "[EMBEDDED DEBUG]: {}".format(str(response.message)),
                    response.reception_time,
                )
            else:
                return response

    def wait_and_handle_response(self):
        try:
            response = self.get_response_from_fifo_and_filter_out_log(
                timeout=CommunicationHandler.RESPONSE_TIMEOUT
            )
        except Empty:
            raise CommunicationHandlerNoResponse(CommunicationHandler.RESPONSE_TIMEOUT)
        return response

    def handle_response(self, response):
        try:
            response = self.RESP_CODE_HANDLER_MAPPER[response.resp_code](response)
        except KeyError:
            raise CommunicationHandlerUnknownResponseException(response)
        return response

    def empty_fifo(self):
        while True:
            try:
                response_raw = self.serial_handler.response_fifo.get(
                    timeout=CommunicationHandler.RESPONSE_TIMEOUT
                )
                if response_raw:
                    self.log("Remaining message in FIFO: {}".format(response_raw))
            except Empty:
                break
