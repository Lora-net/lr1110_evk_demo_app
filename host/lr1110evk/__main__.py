"""
Entry points for LR1110 Demo and Field tests

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

from .Job import (
    Executor,
    Logger,
    ResultLogger,
    ExecutorCriticalException,
)
from .FieldTestPost.Core import GeoLocServiceClientGnss, GeoLocServiceClientWifi
from .SerialExchange.DemoVcpReader import VcpReader
from .AppConfiguration import DemoAppConfiguration
from argparse import ArgumentParser

from datetime import datetime
import pkg_resources
from os import path, makedirs, getcwd
from shutil import copyfile
from serial.serialutil import SerialException


def test_cli():
    version = pkg_resources.get_distribution("lr1110evk").version
    default_top_result_folder = getcwd()
    parser = ArgumentParser(description="Host companion for field tests of LR1110")
    parser.add_argument(
        "--top-results-folder",
        "-f",
        default=default_top_result_folder,
        help="Set the top directory where to store all the subfolders results. Default is current directory.",
    )
    parser.add_argument(
        "--result-folder-name",
        "-r",
        default=None,
        help="Name of the subdirectory that holds results files for the current run. Default is 'fieldTests_' with time appended.",
    )
    parser.add_argument("jobFile")
    parser.add_argument("--version", action="version", version=version)
    args = parser.parse_args()

    date_string = datetime.utcnow().strftime("%Y%m%d-%H%M%S")
    result_folder_name = args.result_folder_name or "fieldTests_{}".format(date_string)
    result_folder_path = path.join(args.top_results_folder, result_folder_name)

    result_filename = path.join(result_folder_path, "results.res")
    log_filename = path.join(result_folder_path, "logging.log")
    job_filename = args.jobFile
    job_filename_no_folder = path.basename(job_filename)

    try:
        makedirs(result_folder_path)
    except FileExistsError:
        print("The directory {} already exists. Aborting...".format(result_folder_path))
        exit(1)
    copyfile(job_filename, path.join(result_folder_path, job_filename_no_folder))

    result_logger = ResultLogger(result_filename)
    log_logger = Logger(log_filename)
    log_logger.print_also_on_stdin = True
    log_logger.log("Start")
    log_logger.log("Job filename: {}".format(job_filename))
    log_logger.log("Log filename: {}".format(log_filename))
    log_logger.log("Result filename: {}".format(result_filename))

    executor = Executor(result_logger, log_logger)
    executor.connect_serial()
    executor.load_jobs_from_file(job_filename)
    try:
        executor.execute()
    except ExecutorCriticalException as exec_crit_exc:
        log_logger.log("Terminating on critical exception: {}".format(exec_crit_exc))
    except KeyboardInterrupt:
        log_logger.log("User Keyboard Interrupt received")
        executor.reset()
    finally:
        executor.stop()
        log_logger.log("Terminating...")
        log_logger.terminate()
        result_logger.terminate()


def entry_point_demo():
    default_device = "/dev/ttyACM0"
    default_baud = 921600
    default_wifi_server_port_number = GeoLocServiceClientWifi.get_default_port_number()
    default_gnss_server_port_number = GeoLocServiceClientGnss.get_default_port_number()
    default_wifi_server_base = GeoLocServiceClientWifi.get_default_base_url()
    default_gnss_server_base = GeoLocServiceClientGnss.get_default_base_url()
    default_dry_run = False
    default_gnss_assisted_scan_coordinate = None
    default_reverse_geo_coding = False

    version = pkg_resources.get_distribution("lr1110evk").version
    parser = ArgumentParser()
    parser.add_argument(
        "approximateGnssServerLocalization",
        help="approximate coordinate sent to the server as initial localization for GNSS localization solver. Format is <latitude>,<longitude>,<altitude>",
    )
    parser.add_argument(
        "exactCoordinate",
        help="Exact coordinate. Sent to servers for error processing. Format is <latitude>,<longitude>,<altitude>",
    )
    parser.add_argument(
        "glsAuthenticationToken",
        help="HTTP header token to authenticate the Wi-Fi requests on LoRa Cloud Geolocation (GLS) server",
    )
    parser.add_argument(
        "-s",
        "--wifi-server-base-url",
        help="Wifi server base address to use (default={}".format(
            default_wifi_server_base
        ),
        default=default_wifi_server_base,
    )
    parser.add_argument(
        "-p",
        "--wifi-server-port",
        type=int,
        help="Port number of the wifi server to contact (default={})".format(
            default_wifi_server_port_number
        ),
        default=default_wifi_server_port_number,
    )
    parser.add_argument(
        "-t",
        "--gnss-server-base-url",
        help="GNSS server base address to use (default={}".format(
            default_gnss_server_base
        ),
        default=default_gnss_server_base,
    )
    parser.add_argument(
        "-q",
        "--gnss-server-port",
        type=int,
        help="Port number of the GNSS server to contact (default={})".format(
            default_gnss_server_port_number
        ),
        default=default_gnss_server_port_number,
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
        "-n",
        "--dry-run",
        action="store_true",
        help="Do not contact geolocalization service (default={})".format(
            default_dry_run
        ),
        default=default_dry_run,
    )
    parser.add_argument(
        "-l",
        "--gnss-assisted-scan-approximate-localization",
        type=str,
        help="Set the estimated localization given to LR1110 for Assisted Start GNSS scan. Format is <latitude>,<longitude>,<altitude> (default is approximateGnssServerLocalization)",
        default=default_gnss_assisted_scan_coordinate,
    )
    parser.add_argument(
        "-r",
        "--reverse-geo-coding",
        action="store_true",
        help="Enable the reverse geo coding information in requests sent to server",
        default=default_reverse_geo_coding,
    )
    parser.add_argument(
        "-F",
        "--fake-date",
        action="store",
        help="Allows to fake the actual date to the one provided. Format is '{}'".format(
            DemoAppConfiguration.PARSE_DATE_FORMAT.replace("%", "%%")
        ),
        default=None,
    )
    parser.add_argument(
        "--verbose", "-v", help="Verbose", action="store_true", default=False
    )
    parser.add_argument("--version", action="version", version=version)
    args = parser.parse_args()

    configuration = DemoAppConfiguration.from_arg_parser(args)

    try:
        reader = VcpReader(configuration)
    except SerialException as serial_exception:
        print("Failed to initialize serial device: '{}'".format(serial_exception))
        exit(1)

    reader.start_read_vcp()

    try:
        reader.read_vcp_forever()
    except SerialException as serial_exception:
        print("Failure while reading serial: '{}'".format(serial_exception))
        exit(2)
    except KeyboardInterrupt:
        print("Bye\n")


def drive_field_tests():
    test_cli()


if __name__ == "__main__":
    test_cli()
