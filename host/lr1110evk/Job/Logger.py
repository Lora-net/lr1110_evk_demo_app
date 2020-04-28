"""
Define Logger classes

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

from os.path import exists
from datetime import datetime
from queue import Queue


class LoggerException(Exception):
    def __init__(self):
        super().__init__()


class LoggerFileExists(LoggerException):
    def __init__(self, filename):
        self.filename = filename

    def __str__(self):
        return "File exists: {}".format(self.filename)


class LoggerEntryBase:
    pass


class LoggerItem(LoggerEntryBase):
    def __init__(self, date, message):
        super(LoggerItem, self).__init__()
        self.date = date
        self.message = message

    def __str__(self):
        return "[{date}] {message}".format(date=self.date, message=self.message)


class LoggerComment(LoggerEntryBase):
    def __init__(self, message):
        super(LoggerComment, self).__init__()
        self.message = message

    def __str__(self):
        return "# {}".format(self.message)


class Logger:
    BUFFER_LIMIT = 20

    def __init__(self, filename):
        self.filename = filename
        self.buffer = Queue()
        self.buffer_limit = Logger.BUFFER_LIMIT
        self.print_also_on_stdin = False

    def start(self):
        if exists(self.filename):
            raise LoggerFileExists(self.filename)

    def write_buffer(self):
        with open(self.filename, "a") as f:
            while not self.buffer.empty():
                buf_entry = self.buffer.get()
                f.write(str(buf_entry) + "\n")

    def terminate(self):
        self.write_buffer()

    def __log_base(self, entry):
        self.buffer.put(entry)
        if self.print_also_on_stdin:
            print(str(entry))
        if self.buffer.qsize() > self.buffer_limit:
            self.write_buffer()

    def log(self, log_message, log_date=None):
        if not log_date:
            log_date = datetime.utcnow()
        entry = LoggerItem(date=log_date, message=log_message)
        self.__log_base(entry)

    def log_comment(self, log_message):
        entry = LoggerComment(message=log_message)
        self.__log_base(entry)


class ResultLogger(Logger):

    NO_RESULT_TOKEN = "No result"
    EXCEPTION_TOKEN = "Exception"

    def log_metadata(self, metadata):
        self.log_comment(metadata)

    def log_no_result(self, job_id, job_counter):
        message = "[{} - {}] {}".format(
            job_counter, job_id, ResultLogger.NO_RESULT_TOKEN
        )
        self.log(message)

    def log_exception(self, job_id, job_counter):
        message = "[{} - {}] {}".format(
            job_counter, job_id, ResultLogger.EXCEPTION_TOKEN
        )
        self.log(message)

    def log_wifi(self, wifi_scan, job_id, job_counter):
        wifi_date = wifi_scan.reception_time

        mac_address_object = wifi_scan.mac_address
        mac_address = mac_address_object.mac_address
        wifi_channel = mac_address_object.wifi_channel.name
        wifi_type = mac_address_object.wifi_type
        rssi = str(mac_address_object.rssi)
        timing_demodulation = str(mac_address_object.timing_demodulation)
        timing_capture = str(mac_address_object.timing_capture)
        timing_correlation = str(mac_address_object.timing_correlation)
        timing_detection = str(mac_address_object.timing_detection)

        wifi_message = "[{} - {}] {}".format(
            job_counter,
            job_id,
            ", ".join(
                [
                    mac_address,
                    wifi_channel,
                    wifi_type,
                    rssi,
                    timing_demodulation,
                    timing_capture,
                    timing_correlation,
                    timing_detection,
                ]
            ),
        )
        self.log(wifi_message, wifi_date)

    def log_gnss(self, gnss_scan, job_id, job_counter):
        gnss_date = gnss_scan.gnss_scan.instant_scan
        gnss_nav_message = gnss_scan.gnss_scan.nav_message
        gnss_radio_timing = str(gnss_scan.gnss_scan.radio_timing_ms)
        gnss_computation_timing = str(gnss_scan.gnss_scan.computation_timing_ms)
        elapsed_s = "0"

        gnss_message = "[{} - {}] {}".format(
            job_counter,
            job_id,
            ", ".join(
                [
                    gnss_nav_message,
                    elapsed_s,
                    gnss_radio_timing,
                    gnss_computation_timing,
                ]
            ),
        )
        self.log(gnss_message, gnss_date)
