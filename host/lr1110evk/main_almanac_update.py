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
from argparse import ArgumentParser
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


def entry_point_update_almanac():
    default_device = "/dev/ttyACM0"
    default_baud = 921600
    default_log_filename = "log.log"
    default_url_base = "https://gls.loracloud.com"

    version = pkg_resources.get_distribution("lr1110evk").version
    parser = ArgumentParser()
    parser.add_argument(
        "glsToken", help="Authentication token to the GeoLocation Server (GLS)",
    )
    parser.add_argument(
        "-u",
        "--url-base",
        help="Base URL to use when fetching almanac. Default is {}".format(
            default_url_base
        ),
        default=default_url_base,
    )
    parser.add_argument(
        "-f",
        "--almanac-file",
        help="Get the almanac information from a file instead of downloading it from web API. In this case the GLS token is not used",
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

        log_logger.log("Fetching almanac data from url {}...".format(args.url_base))
        full_url_api = "{}/api/v3/almanac/full".format(args.url_base)
        request_header = {
            "Ocp-Apim-Subscription-Key": args.glsToken,
            "Content-Type": "application/json",
        }
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
