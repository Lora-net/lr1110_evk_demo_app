"""
Define serial handler for field test class

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

from serial import Serial
from serial.tools import list_ports
from serial.serialutil import SerialException
from threading import Thread, Event
from queue import Queue
from .Responses import ResponseRaw
from datetime import datetime


class SerialHandlerException(Exception):
    pass


class SerialHandlerExceptionNoPorts(SerialHandlerException):
    def __init__(self, regexp):
        super().__init__()
        self.regexp = regexp

    def __str__(self):
        return "No port connected that could be STM32 (used regexp '{}')".format(
            self.regexp
        )


class SerialHandlerExceptionTooManyPorts(SerialHandlerException):
    def __init__(self, regexp, ports):
        super().__init__()
        self.ports = ports
        self.regexp = regexp

    def __str__(self):
        return "Too many ports could be the STM32 (used regexp '{}'): [{}]".format(
            self.regexp, ", ".join([port.name for port in self.ports])
        )


class SerialHandlerExceptionReadTimeout(SerialHandlerException):
    def __init__(self, expected_n_bytes, data_read):
        self.expected_n_bytes = expected_n_bytes
        self.data_read = data_read

    def __str__(self):
        return "Timeout while reading serial. Expected {} byte(s), got '{}' (len: {})".format(
            self.expected_n_bytes, self.data_read, len(self.data_read)
        )


class SerialHandlerExceptionDisconnect(SerialHandlerException):
    def __str__(self):
        return "Serial handler lost contact with opened port. Did I get disconnected?"


class SerialHanlerEmbeddedNotSetException(SerialHandlerException):
    def __str__(self):
        return "The embedded side has not been switched to field test mode yet"


class SerialHandler:
    DISCOVER_PORT_REGEXP = "(STM.*)|(374B)"
    SERIAL_BAUDRATE = 921600
    SERIAL_READ_TIMEOUT_S = 1
    TEST_HOST_MESSAGE = b"!TEST_HOST"
    TEST_HOST_FIELD_TEST_RESPONSE = b"fieldglog\x00"

    def __init__(self):
        self.serial_port = None
        self.read_thread = Thread(
            name="SerialRead", target=SerialHandler.read_command, args=(self,)
        )
        self.read_thread_run = Event()
        self.response_fifo = Queue()
        self.is_embedded_set_to_field_test = Event()

    def set_serial_port(self, device):
        self.serial_port = Serial(device)
        self.serial_port.baudrate = SerialHandler.SERIAL_BAUDRATE
        self.serial_port.timeout = SerialHandler.SERIAL_READ_TIMEOUT_S

    def open(self):
        self.serial_port.open()

    def close(self):
        self.serial_port.close()

    def start_read(self):
        self.read_thread_run.clear()
        self.read_thread.start()

    def stop_read(self):
        self.read_thread_run.set()
        self.read_thread.join()

    @staticmethod
    def discover(regexp):
        ports = list(list_ports.grep(SerialHandler.DISCOVER_PORT_REGEXP))
        if not ports:
            raise SerialHandlerExceptionNoPorts(regexp)
        if len(ports) > 1:
            raise SerialHandlerExceptionTooManyPorts(regexp, ports)
        return ports[0]

    def send(self, data: bytes):
        if not self.is_embedded_set_to_field_test.is_set():
            raise SerialHanlerEmbeddedNotSetException()
        self.serial_port.write(data)
        send_time = datetime.utcnow()
        return send_time

    def send_swith_embedded_to_field_test_mode_response(self):
        self.serial_port.write(SerialHandler.TEST_HOST_FIELD_TEST_RESPONSE)

    def read_command(self):
        while not self.read_thread_run.is_set():
            received_response = False
            try:
                received_response = self.read_one_response_or_none()
            except SerialHandlerExceptionReadTimeout:
                print("Error: timeout on received command")
            except SerialHandlerExceptionDisconnect:
                print("Critical error: serial lost device. Did it got disconnected?")
                break
            if received_response:
                if received_response.resp_code == SerialHandler.TEST_HOST_MESSAGE[0:2]:
                    self.send_swith_embedded_to_field_test_mode_response()
                    self.is_embedded_set_to_field_test.set()
                else:
                    # print("Serial handler received: {}".format(received_response))
                    self.response_fifo.put(received_response)
        print("[Serial Handler] Leaving runtime")

    def read_n_bytes_or_timeout(self, n_bytes):
        try:
            data_read = self.serial_port.read(n_bytes)
        except SerialException:
            raise SerialHandlerExceptionDisconnect()
        if len(data_read) < n_bytes:
            raise SerialHandlerExceptionReadTimeout(n_bytes, data_read)
        return data_read

    def read_one_response_or_none(self):
        try:
            resp_code = self.read_n_bytes_or_timeout(2)
        except SerialHandlerExceptionReadTimeout:
            # It is ok to timeout on reception of the resp_code,
            # but once the resp_code is received, it is an exception
            # to not receive the complete commands
            return None
        receive_time = datetime.utcnow()
        # Dirty trick to receive the !TEST_HOST command
        if resp_code == b"!T":
            payload_size = 9
        else:
            payload_size = int.from_bytes(
                self.read_n_bytes_or_timeout(2), byteorder="little"
            )
        if payload_size > 0:
            payload = self.serial_port.read(payload_size)
        else:
            payload = b""
        response = ResponseRaw(
            resp_code=resp_code, payload=payload, receive_time=receive_time
        )
        return response
