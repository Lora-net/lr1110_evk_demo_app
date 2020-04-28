"""
Define serial handler for USB connection check class

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

from threading import Thread
from .SerialHandler import SerialHandler


class SerialHandlerConnectionTest(SerialHandler):
    TEST_HOST_CONNECTION_TEST_RESPONSE = b"testdglog\x00"
    EXPECTED_RESPONSE_FROM_EMBEDDED = b"It works !"

    def __init__(self):
        super().__init__()
        self.read_thread = Thread(
            name="SerialRead",
            target=SerialHandlerConnectionTest.read_command,
            args=(self,),
        )

    def read_command(self):
        print("Waiting for embedded to send command...")
        while not self.read_thread_run.is_set():
            line = self.read_one_line()
            if line:
                if line.startswith(SerialHandler.TEST_HOST_MESSAGE.decode("utf-8")):
                    # Test message received
                    self.serial_port.write(
                        SerialHandlerConnectionTest.TEST_HOST_CONNECTION_TEST_RESPONSE
                    )
                    print("Got message from embedded. Sending question...")
                elif line.startswith(
                    SerialHandlerConnectionTest.EXPECTED_RESPONSE_FROM_EMBEDDED.decode(
                        "utf-8"
                    )
                ):
                    print("Got response from embedded. Communication ok.")
                else:
                    print(
                        "Unexpected data received from embedded: '{}'. Communication KO!".format(
                            line
                        )
                    )
        print("Leaving runtime")

    def read_one_line(self):
        try:
            line = self.serial_port.readline().decode("ascii")
        except UnicodeDecodeError:
            print("Error on USB line. Communication KO!")
            line = None
        return line


def launch_test():
    from time import sleep

    serial_handler = SerialHandlerConnectionTest()
    port = serial_handler.discover(SerialHandler.DISCOVER_PORT_REGEXP)
    serial_handler.set_serial_port(port.device)

    print("Started")
    serial_handler.start_read()

    try:
        while True:
            sleep(2)
    except KeyboardInterrupt:
        serial_handler.stop_read()
    finally:
        print("Bye")


def entry_point_connection_tester():
    import pkg_resources
    from argparse import ArgumentParser

    version = pkg_resources.get_distribution("lr1110evk").version
    parser = ArgumentParser(
        description="Companion software of LR1110 Dev Kit. Check the USB connection with a dedicated embedded software that do not require the LR1110 shield to be connected."
    )
    parser.add_argument("--version", action="version", version=version)
    parser.parse_args()

    launch_test()


if __name__ == "__main__":
    launch_test()
