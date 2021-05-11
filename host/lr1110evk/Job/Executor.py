"""
Define field test executor class

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

from .JobReader import JobReader
from ..SerialExchange import (
    SerialHandler,
    CommunicationHandler,
    CommunicationHandlerException,
    CommunicationHandlerSerialNotListeningException,
)
from .JobExecutor import JobExecutor, JobExecutorException, JobResetFailed
from .JobJsonValidator import JobValidator
from time import sleep


class ExecutorException(Exception):
    pass


class ExecutorCriticalException(ExecutorException):
    def __init__(self, exception):
        self.exception = exception

    def __str__(self):
        return "Critical job executor exception: {}".format(self.exception)


class Executor:
    def __init__(self, result_logger, debug_logger):
        self.job_reader = JobReader()
        self.serial_handler = SerialHandler()
        self.communication_handler = CommunicationHandler(
            self.serial_handler, debug_logger
        )
        self.job_executor = JobExecutor(self.communication_handler, debug_logger)
        self.result_logger = result_logger
        self.debug_logger = debug_logger
        self.job_execution_counter = 0

    def stop(self):
        self.communication_handler.stop()

    def load_jobs_from_file(self, job_filename):
        JobValidator.validate_job_json_file(job_filename)
        self.job_reader.read_from_file(job_filename)

    def load_jobs_from_job_list(self, job_list, infinite_execution):
        self.job_reader.jobs.extend(job_list)
        self.job_reader.infinite_execution = infinite_execution

    def connect_serial(self):
        port = SerialHandler.discover(SerialHandler.DISCOVER_PORT_REGEXP)
        device = port.device
        self.serial_handler.set_serial_port(device)

    def execute(self):
        self.communication_handler.start()
        self.communication_handler.wait_embedded_to_be_configured_for_field_test()
        self.get_and_save_version_info()
        if self.job_reader.infinite_execution:
            # In this case, the list of jobs is executed in a row and never stops, util user press CTRL-c.
            # If the jobs has been configured with a scan_interval different from 0, then a wait of the
            # configured amount of seconds is executed before starting the next job list.
            while True:
                self.execute_jobs()
                if self.job_reader.scan_interval > 0:
                    self.log(
                        "Wait {} second(s) before starting next job list...".format(
                            self.job_reader.scan_interval
                        )
                    )
                    sleep(self.job_reader.scan_interval)
        else:
            self.execute_jobs()
        self.stop()

    def reset(self):
        self.log("Attempting reset")
        try:
            self.job_executor.reset()
        except JobResetFailed:
            self.exception("Reset target while stop failed")

    def get_and_save_version_info(self):
        version = self.job_executor.get_version_info()
        self.result_logger.log_metadata("version: {}".format(version.to_metadata()))

    def execute_jobs(self):
        for job in self.job_reader.jobs:
            self.job_execution_counter += 1
            self.log(
                "Execute job #{}: [{}]".format(self.job_execution_counter, str(job))
            )
            execute_post_reset = False
            try:
                results = self.job_executor.execute_job(job)
            except JobExecutorException as job_exc:
                # That's a high level exception: link is ok, but command is
                # unknown
                self.result_logger.log_exception(job.id, self.job_execution_counter)
                self.exception("Job failed with error: {}".format(str(job_exc)))
                execute_post_reset = True
            except CommunicationHandlerSerialNotListeningException as com_not_listening:
                self.result_logger.log_exception(job.id, self.job_execution_counter)
                execute_post_reset = False  # We don't try a post reset here as the exception comes from the fact the thread managing the serial is dead
                self.exception(
                    "Communication failure: {}".format(str(com_not_listening))
                )
                raise ExecutorCriticalException(com_not_listening)
            except CommunicationHandlerException as com_hand_exc:
                # That's a low level exception: link is not ok
                self.result_logger.log_exception(job.id, self.job_execution_counter)
                execute_post_reset = True
                self.exception("Communication failure: {}".format(str(com_hand_exc)))
            else:
                # Save results
                if results:
                    self.log(
                        "[RESULTS] Job {}({}): {} result(s)\n  - {}".format(
                            job.id,
                            job.name,
                            len(results),
                            "\n  - ".join(["{}".format(res) for res in results]),
                        )
                    )
                self.save_results(results, job.id, self.job_execution_counter)
            if execute_post_reset:
                try:
                    self.communication_handler.empty_fifo()
                    self.reset()
                except JobResetFailed:
                    self.exception("Soft reset failed")
            self.log("Terminate job: {}".format(str(job)))

    def save_results(self, results, job_id, job_counter):
        if not results:
            self.result_logger.log_no_result(job_id, job_counter)
        for result in results:
            try:
                self.result_logger.log_wifi(result, job_id, job_counter)
            except AttributeError:
                self.result_logger.log_gnss(result, job_id, job_counter)

    def log(self, msg):
        self.debug_logger.log("[LOG] {}".format(msg))

    def exception(self, msg):
        self.debug_logger.log("[EXCEPTION] {}".format(msg))
