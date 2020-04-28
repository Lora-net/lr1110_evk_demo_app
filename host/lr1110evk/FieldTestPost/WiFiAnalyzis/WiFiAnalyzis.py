"""
Define Wi-Fi analyzis class

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

from ..Core import FileReader
from lr1110evk.BaseTypes import ScannedMacAddress, WifiChannels


class WiFiAnalyzis:
    def __init__(self):
        self.wifi_data = None

    def parse_measurement_file(self, result_filename):
        all_data = FileReader.parse_file(result_filename)
        self.wifi_data = [
            data for data in all_data if WiFiAnalyzis.is_result_line_wifi(data)
        ]

    def mac_addresses_count_over_time(self):
        mac_count_per_time = list()
        group_results = FileReader.generate_result_groups(self.wifi_data)
        for date_result in group_results:
            date_result_list = list(date_result[1])
            date = date_result_list[0].date
            wifi_results = [wifi_line.scan_info for wifi_line in date_result_list]
            count_per_channels = WiFiAnalyzis.mac_addresses_per_channels(wifi_results)
            mac_count_per_time.append((date, count_per_channels))
        return mac_count_per_time

    @staticmethod
    def compute_sliding_average_per_time(mac_count_per_time, time_window):
        init_time_data = mac_count_per_time[0][0]
        end_time_data = mac_count_per_time[-1][0]
        t0_average = init_time_data + time_window
        t_start = init_time_data
        t_end = t0_average
        average_mac_count_per_time = list()
        number_of_scan_per_time = list()
        while t_end <= end_time_data:
            count_per_time_in_window = WiFiAnalyzis.get_point_between_date(
                mac_count_per_time, t_start, t_end
            )
            number_of_scan = len(count_per_time_in_window)
            count_per_channels = {
                channel: sum(
                    [counts[1][channel] for counts in count_per_time_in_window]
                )
                / number_of_scan
                for channel in WifiChannels.WIFI_CHANNELS
            }
            average_mac_count_per_time.append((t_end, count_per_channels))
            number_of_scan_per_time.append((t_end, number_of_scan))
            t_start += time_window
            t_end += time_window
        return number_of_scan_per_time, average_mac_count_per_time

    @staticmethod
    def get_point_between_date(mac_count_per_time, start_time, end_time):
        return [
            mac_per_time
            for mac_per_time in mac_count_per_time
            if start_time <= mac_per_time[0] <= end_time
        ]

    @staticmethod
    def mac_addresses_per_channels(wifi_result_lines):
        channel_counter = dict.fromkeys(WifiChannels.WIFI_CHANNELS, 0)
        for wifi in wifi_result_lines:
            scanned_channel = wifi.wifi_channel
            channel_counter[scanned_channel] += 1
        return channel_counter

    @staticmethod
    def is_result_line_wifi(result_line):
        return isinstance(result_line.scan_info, ScannedMacAddress)
