"""
Define single Field test job executor class

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

from ..SerialExchange.Commands import (
    CommandConfigure,
    CommandStart,
    CommandFetchResults,
    CommandSetDateLoc,
    CommandReset,
    CommandGetVersion,
    CommandGetAlmanacDates,
)
from ..SerialExchange.CommunicationHandler import (
    CommunicationHandlerException,
    CommunicationHandlerNoResponse,
)
from .GnssDateLocBuilder import GnssDateLocBuilder
from ..BaseTypes import Coordinate, Version
from datetime import datetime, timedelta


class JobExecutorException(Exception):
    def __init__(self, job):
        self.failed_job = job


class JobConfigurationFailed(JobExecutorException):
    def __init__(self, job):
        super().__init__(job)

    def __str__(self):
        return "Configuration failed on job: {}".format(self.failed_job)


class JobStartFailed(JobExecutorException):
    def __init__(self, job):
        super().__init__(job)

    def __str__(self):
        return "Start failed on job: {}".format(self.failed_job)


class JobNoEventReceivedException(JobExecutorException):
    def __init__(self, job, timeout):
        super().__init__(job)
        self.timeout = timeout

    def __str__(self):
        return "Event wait timeout ({} second(s)) for job {}".format(
            self.timeout, self.failed_job
        )


class JobExecutorMismatchComResp(JobExecutorException):
    def __init__(self, job, command_sent, response_received):
        super().__init__(job)
        self.command_sent = command_sent
        self.response_received = response_received

    def __str__(self):
        return "Mismatch between command '{}' and response received '{}' for job '{}'".format(
            self.command_sent, self.response_received, self.failed_job
        )


class JobResetFailed(Exception):
    def __init__(self):
        pass

    def __str__(self):
        return "Reset request failed"


class JobExecutor:
    EVENT_WAIT_TIMEOUT_WIFI_S = 10
    EVENT_WAIT_TIMEOUT_GNSS_ASSISTED_S = 40
    EVENT_WAIT_TIMEOUT_GNSS_AUTONOMOUS_S = 140

    def __init__(self, communication_handler, debug_logger):
        self.communication_handler = communication_handler
        self.debug_logger = debug_logger

    @staticmethod
    def compute_timeout(job):
        timeout_s = 0
        if job.has_gnss_assisted:
            timeout_s += JobExecutor.EVENT_WAIT_TIMEOUT_GNSS_ASSISTED_S
        if job.has_wifi:
            timeout_s += (
                (job.wifi_timeout / 1000)
                * len(job.wifi_types)
                * len(job.wifi_channels)
                * job.wifi_nbr_retrials
            )
        if job.has_gnss_autonomous:
            timeout_s += JobExecutor.EVENT_WAIT_TIMEOUT_GNSS_AUTONOMOUS_S
        return timeout_s

    def configure_job(self, job):
        self.log("Configuring...")
        conf_command = JobExecutor.build_conf_command_from_job(job)
        conf_command_sent, conf_response = self.handle_and_log_command(conf_command)
        if not JobExecutor.is_exchange_valid(conf_command_sent, conf_response):
            raise JobExecutorMismatchComResp(job, conf_command_sent, conf_response)
        if not conf_response.ack_status:
            raise JobConfigurationFailed(job)

        # If there is a gnss assisted or autonomous, the date and assisted location must be updated
        if job.has_gnss_assisted or job.has_gnss_autonomous:
            self.log("Updating date and loc...")
            set_date_loc_command = JobExecutor.build_set_date_loc_command_from_job(job)
            (
                set_date_loc_command_sent,
                set_date_loc_response,
            ) = self.handle_and_log_command(set_date_loc_command)
            if not JobExecutor.is_exchange_valid(
                set_date_loc_command_sent, set_date_loc_response
            ):
                raise JobExecutorMismatchComResp(
                    job, set_date_loc_command_sent, set_date_loc_response
                )

    def star_job(self, job):
        self.log("Starting...")
        start_command = CommandStart()
        start_command_sent, start_response = self.handle_and_log_command(start_command)
        if not JobExecutor.is_exchange_valid(start_command_sent, start_response):
            raise JobExecutorMismatchComResp(job, start_command_sent, start_response)
        if not start_response.ack_status:
            raise JobStartFailed(job)

    def wait_event_job(self, job):
        timeout_s = JobExecutor.compute_timeout(job)
        self.log("Waiting for event (timeout: {} second(s))...".format(timeout_s))
        got_event = self.wait_event_or_timeout(timeout_s)
        if not got_event:
            raise JobNoEventReceivedException(job, timeout_s)

    def store_result_job(self, job):
        self.log("Fetching results...")
        fetch_result_command = CommandFetchResults()
        fetch_result_command_sent, fetch_result_response = self.handle_and_log_command(
            fetch_result_command
        )
        if not JobExecutor.is_exchange_valid(
            fetch_result_command_sent, fetch_result_response
        ):
            raise JobExecutorMismatchComResp(
                job, fetch_result_command_sent, fetch_result_response
            )
        nbr_result_to_fetch = fetch_result_response.nbr_results
        results = self.receive_results(nbr_result_to_fetch)
        return results

    def execute_job(self, job):
        # 0. Reset if required
        if job.reset_before_job_start:
            self.reset()

        # if not job.has_gnss_autonomous and not job.has_gnss_assisted and not job.has_wifi:
        #     self.log(
        #         "Job is configured to run nothing: will not send commands and return no results")
        #     return list()

        # 1. Configure
        self.configure_job(job)

        # 2. Start
        self.star_job(job)

        # 3. Wait for event
        self.wait_event_job(job)

        # 4. Fetch results
        results = self.store_result_job(job)
        return results

    def get_version_info(self):
        almanac_ages = self.get_almanac_per_satellites()
        get_version_command = CommandGetVersion()
        command_sent, response_version = self.handle_and_log_command(
            get_version_command
        )
        if JobExecutor.is_exchange_valid(command_sent, response_version):
            version = Version(
                host_version=Version.get_host_version(),
                demo_version=response_version.software_version,
                driver_version=response_version.driver_version,
                lr1110_version=response_version.lr1110_firmware_version,
                almanac_crc=response_version.almanac_crc,
                almanac_ages=almanac_ages,
                chip_uid=response_version.chip_uid,
            )
            self.log("Software version: {}".format(response_version.software_version))
            self.log(
                "LR1110 Hardware version: {}".format(response_version.lr1110_hw_version)
            )
            self.log(
                "LR1110 Firmware version: {}".format(
                    response_version.lr1110_firmware_version
                )
            )
        else:
            version = Version(
                host_version="Unknown",
                demo_version="Unknown",
                driver_version="Unknown",
                lr1110_version="Unknown",
                almanac_crc="Unknown",
                almanac_ages=dict(),
                chip_uid="Unknown",
            )
            self.log("Unknown software/LR1110 versions")
        return version

    def get_almanac_per_satellites(self):
        get_almanac_command = CommandGetAlmanacDates()
        command_sent, response_almanac = self.handle_and_log_command(
            get_almanac_command
        )
        if JobExecutor.is_exchange_valid(command_sent, response_almanac):
            almanac_ages = response_almanac.almanac_age_per_satellites
            self.log("Almanac ages: {}".format(almanac_ages))
        else:
            almanac_ages = None
            self.log("Unknown almanac ages")
        return almanac_ages

    def reset(self):
        reset_command = CommandReset()
        try:
            reset_command_sent, reset_response = self.handle_and_log_command(
                reset_command
            )
        except CommunicationHandlerException:
            raise JobResetFailed()
        if not JobExecutor.is_exchange_valid(reset_command_sent, reset_response):
            raise JobResetFailed()
        if not reset_response.ack_status:
            raise JobResetFailed()

    def receive_results(self, nbr_results):
        import time

        results = list()
        for _ in range(0, nbr_results):
            try:
                result = self.communication_handler.wait_and_handle_response()
            except CommunicationHandlerNoResponse:
                continue
            results.append(result)
            time.sleep(0.01)
        return results

    def handle_and_log_command(self, command):
        command_sent, response_received = self.communication_handler.handle_exchange(
            command
        )
        self.log("Command: {}".format(command_sent))
        self.log("Response: {}".format(response_received))
        return command_sent, response_received

    @staticmethod
    def is_exchange_valid(command, response):
        return command.get_com_code() == response.get_response_code()

    @staticmethod
    def build_conf_command_from_job(job):
        command_configure = CommandConfigure()
        command_configure.wifi_channels = job.wifi_channels
        command_configure.wifi_types = job.wifi_types
        command_configure.wifi_enable_mode = job.wifi_enable_mode
        command_configure.wifi_nbr_retrials = job.wifi_nbr_retrials
        command_configure.wifi_max_results_per_scan = job.wifi_max_results
        command_configure.wifi_timeout = job.wifi_timeout
        command_configure.wifi_mode = job.wifi_mode
        command_configure.gnss_autonomous_enable = job.gnss_autonomous_enable
        command_configure.gnss_autonomous_option = job.gnss_autonomous_option
        command_configure.gnss_autonomous_capture_mode = (
            job.gnss_autonomous_capture_mode
        )
        command_configure.gnss_autonomous_nb_satellite = (
            job.gnss_autonomous_nb_satellite
        )
        command_configure.gnss_autonomous_antenna_selection = (
            job.gnss_autonomous_antenna_selection
        )
        command_configure.gnss_autonomous_constellation_mask = (
            job.gnss_autonomous_constellation_mask
        )
        command_configure.gnss_assisted_enable = job.gnss_assisted_enable
        command_configure.gnss_assisted_option = job.gnss_assisted_option
        command_configure.gnss_assisted_capture_mode = job.gnss_assisted_capture_mode
        command_configure.gnss_assisted_nb_satellite = job.gnss_assisted_nb_satellite
        command_configure.gnss_assisted_antenna_selection = (
            job.gnss_assisted_antenna_selection
        )
        command_configure.gnss_assisted_constellation_mask = (
            job.gnss_assisted_constellation_mask
        )
        return command_configure

    @staticmethod
    def build_set_date_loc_command_from_job(job):
        command_set_date_loc = CommandSetDateLoc()
        command_set_date_loc.gps_time = GnssDateLocBuilder.get_now_gps()
        command_set_date_loc.gps_assisted_location = Coordinate(
            latitude=job.assisted_coordinate.latitude,
            longitude=job.assisted_coordinate.longitude,
            altitude=job.assisted_coordinate.altitude,
        )
        return command_set_date_loc

    def wait_event_or_timeout(self, timeout_s):
        timeout_s = timedelta(seconds=timeout_s)
        start_time = datetime.utcnow()
        elapsed_seconds = 0
        while not self.communication_handler.has_received_event():
            elapsed_seconds = datetime.utcnow() - start_time
            if elapsed_seconds > timeout_s:
                self.log(
                    "Did not receive event. Timeout: {} second(s)s".format(timeout_s)
                )
                return False
        elapsed_seconds = datetime.utcnow() - start_time
        self.log("Received event. Waited {} second(s)".format(elapsed_seconds))
        return True

    def log(self, message):
        self.debug_logger.log("[JobExecutor] {}".format(message))

    def log_exception(self, message):
        self.debug_logger.log("[JobExecutor Exception] {}".format(message))
