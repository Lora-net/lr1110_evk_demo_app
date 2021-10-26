"""
Entry points for LR1110 Almanac update

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

import pkg_resources
from argparse import ArgumentParser, RawDescriptionHelpFormatter
from .Job import (
    UpdateAlmanacCheckFailure,
    UpdateAlmanacDownloadFailure,
    UpdateAlmanacWrongResponseException,
    UpdateAlmanacJob,
    Logger,
    LoggerException,
)
from .SerialExchange import (
    SerialHandler,
    CommunicationHandler,
    SerialHanlerEmbeddedNotSetException,
)


class ServerSolver:
    HUMAN_READABLE_NAME = None
    DEFAULT_DOMAIN = None
    DEFAULT_PATH = None
    HEADER_AUTH_TOKEN = None
    HEADER_CONTENT_TYPE_TOKEN = None

    def __init__(self, domain, path):
        self.__domain = domain
        self.__path = path

    def build_url(self):
        return "{}/{}".format(self.domain, self.path)

    def build_header(self, auth_value):
        return {
            self.get_header_auth_token(): auth_value,
            self.get_header_content_type_token(): "application/json",
        }

    @property
    def domain(self):
        return self.__domain

    @domain.setter
    def domain(self, domain):
        self.__domain = domain

    @property
    def path(self):
        return self.__path

    @path.setter
    def path(self, path):
        self.__path = path

    @classmethod
    def build_default_server_solver(cls):
        return cls(
            domain=cls.DEFAULT_DOMAIN,
            path=cls.DEFAULT_PATH,
        )

    @classmethod
    def get_header_content_type_token(cls):
        return cls.HEADER_CONTENT_TYPE_TOKEN

    @classmethod
    def get_header_auth_token(cls):
        return cls.HEADER_AUTH_TOKEN

    @classmethod
    def get_human_readable_name(cls):
        return cls.HUMAN_READABLE_NAME

    def __str__(self):
        return "{} ({})".format(self.get_human_readable_name(), self.build_url())


class GlsServerSolver(ServerSolver):
    HUMAN_READABLE_NAME = "GLS"
    DEFAULT_DOMAIN = "https://gls.loracloud.com"
    DEFAULT_PATH = "api/v3/almanac/full"
    HEADER_AUTH_TOKEN = "Ocp-Apim-Subscription-Key"
    HEADER_CONTENT_TYPE_TOKEN = "Content-Type"


class DasServerSolver(ServerSolver):
    HUMAN_READABLE_NAME = "DAS"
    DEFAULT_DOMAIN = "https://das.loracloud.com"
    DEFAULT_PATH = "api/v1/almanac/full"
    HEADER_AUTH_TOKEN = "Authorization"
    HEADER_CONTENT_TYPE_TOKEN = "Accept"


def entry_point_update_almanac():
    default_device = "/dev/ttyACM0"
    default_baud = 921600
    default_log_filename = "log.log"
    default_solver = DasServerSolver.build_default_server_solver()

    description = """EVK Demo App companion software that update almanac of LR1110 device.
    This software can fetch almanac from two servers:
       - GeoLocation Server (GLS): default URL is {}
       - Device and Application Server (DAS): default URL is {}
    By default, {} server is used.

    Optionnaly, almanac can also be read from file, which avoid contacting a server.""".format(
        GlsServerSolver.build_default_server_solver().build_url(),
        DasServerSolver.build_default_server_solver().build_url(),
        default_solver,
    )

    version = pkg_resources.get_distribution("lr1110evk").version
    parser = ArgumentParser(
        description=description, formatter_class=RawDescriptionHelpFormatter
    )
    parser.add_argument(
        "token",
        help="Authentication token to the selected server",
    )
    parser.add_argument(
        "-g",
        "--gls",
        help="Use GLS server instead of DAS",
        action="store_true",
    )
    parser.add_argument(
        "-u",
        "--url-domain",
        help="Modify the domain name of the URL to use when fetching almanac",
        default=None,
    )
    parser.add_argument(
        "-f",
        "--almanac-file",
        help="Get the almanac information from a file instead of downloading it from web API. In this case the authentication server token is not used",
        default=None,
    )
    parser.add_argument(
        "-d",
        "--device-address",
        help="Address of the device connecting the lr1110 (default={})".format(
            default_device
        ),
        default=default_device,
    )
    parser.add_argument(
        "-b",
        "--device-baud",
        help="Baud for communication with the lr1110 (default={})".format(default_baud),
        default=default_baud,
    )
    parser.add_argument(
        "-l",
        "--log-filename",
        help="File to use to store the log (default={})".format(default_log_filename),
        default=default_log_filename,
    )
    parser.add_argument("--version", action="version", version=version)
    args = parser.parse_args()

    # Check whether user wants GLS or DAS
    if args.gls is True:
        solver = GlsServerSolver.build_default_server_solver()
    else:
        solver = default_solver

    # Checks whether user wants to override URL domain or path
    if args.url_domain is not None:
        solver.domain = args.url_domain

    log_logger = Logger(args.log_filename)
    log_logger.print_also_on_stdin = True

    log_logger.log("Starting")

    if args.almanac_file:
        log_logger.log("Reading file...")
        with open(args.almanac_file, "rb") as almanac_file:
            almanac_bin = almanac_file.read()
    else:
        import requests
        from base64 import decodebytes

        full_url_api = solver.build_url()
        log_logger.log("Fetching almanac data from url {}...".format(full_url_api))
        request_header = solver.build_header(args.token)

        almanac_server_response = requests.get(full_url_api, headers=request_header)

        try:
            almanac_server_response.raise_for_status()
        except requests.exceptions.HTTPError as http_error:
            log_logger.log(
                "Error contacting server: '{}': '{}'".format(
                    http_error, almanac_server_response.text
                )
            )
            return

        raw_almanac = almanac_server_response.json()["result"]["almanac_image"].encode()
        almanac_bin = decodebytes(raw_almanac)

    serial_handler = SerialHandler()
    if not args.device_address:
        port = SerialHandler.discover(SerialHandler.DISCOVER_PORT_REGEXP)
        device = port.device
    else:
        device = args.device_address
    serial_handler.set_serial_port(device)

    communication_handler = CommunicationHandler(serial_handler, log_logger)
    communication_handler.start()
    communication_handler.wait_embedded_to_be_configured_for_field_test(3)

    almanac_crc = int.from_bytes(almanac_bin[3:7], byteorder="little")
    log_logger.log("CRC is {}".format(almanac_crc))

    update_almanac_job = UpdateAlmanacJob(
        communication_handler=communication_handler,
        almanac_bytestream=almanac_bin,
        expected_crc=almanac_crc,
        logger=log_logger,
    )

    try:
        update_almanac_job.execute_update()
    except UpdateAlmanacCheckFailure:
        log_logger.log("Final CRC check failed")
    except UpdateAlmanacDownloadFailure:
        log_logger.log("Download to chip failed")
    except SerialHanlerEmbeddedNotSetException:
        log_logger.log(
            "Embedded seems connected but did not respond. Have you reset it?"
        )
    finally:
        communication_handler.stop()
        log_logger.log("Bye")
        log_logger.terminate()
