/**
 * @file      communication_print_only.cpp
 *
 * @brief     Implementation of the communication print only interface.
 *
 * Revised BSD License
 * Copyright Semtech Corporation 2020. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include "communication_print_only.h"

CommunicationPrintOnly::CommunicationPrintOnly( ) {}

CommunicationPrintOnly::~CommunicationPrintOnly( ) {}

void CommunicationPrintOnly::Runtime( ) { return; }

void CommunicationPrintOnly::vLog( const char* fmt, va_list argp ) { vprintf( fmt, argp ); }

bool CommunicationPrintOnly::GetDateAndApproximateLocation( uint32_t& gps_second, float& latitude, float& longitude,
                                                            float& altitude )
{
    return false;
}

bool CommunicationPrintOnly::GetResults( float& latitude, float& longitude, float& altitude, float& accuracy,
                                         char* geo_coding, const uint8_t geo_coding_max_length )
{
    return false;
}

bool CommunicationPrintOnly::HasNewCommand( ) const { return false; }

CommandInterface* CommunicationPrintOnly::FetchCommand( ) { return nullptr; }

void CommunicationPrintOnly::Store( const demo_wifi_scan_all_results_t& wifi_results )
{
    printf( "# %u Wi-Fi result(s):\n", wifi_results.nbrResults );
    for( uint8_t result_index = 0; result_index < wifi_results.nbrResults; result_index++ )
    {
        demo_wifi_scan_single_result_t local_result = wifi_results.results[result_index];

        printf(
            "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x, CHANNEL_%i, %s, %i, %u, %u, "
            "%u, "
            "%u\n",
            local_result.mac_address[0], local_result.mac_address[1], local_result.mac_address[2],
            local_result.mac_address[3], local_result.mac_address[4], local_result.mac_address[5], local_result.channel,
            CommunicationInterface::WifiTypeToStr( local_result.type ), local_result.rssi,
            wifi_results.timings.demodulation_us, wifi_results.timings.rx_capture_us,
            wifi_results.timings.rx_correlation_us, wifi_results.timings.rx_detection_us );
    }
}

void CommunicationPrintOnly::Store( const demo_gnss_all_results_t& gnss_results, uint32_t delay_since_capture )
{
    printf( "# GNSS result captured %u second(s) ago:\n", delay_since_capture );
    for( uint16_t index = 0; index < gnss_results.nav_message.size; index++ )
    {
        printf( "%02x", gnss_results.nav_message.message[index] );
    }
    printf( ", %u, %u, %u", delay_since_capture, gnss_results.timings.radio_ms, gnss_results.timings.computation_ms );
    printf( "\n" );
}

void CommunicationPrintOnly::Store( const version_handler_t& version )
{
    switch( version.device_type )
    {
    case VERSION_DEVICE_TRANSCEIVER:
    {
        printf(
            "Software version: %s\nDriver version: %s\nChip version: 0x%04x\nAlmanac date: 0x%x\nChip UID: "
            "%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n",
            version.version_sw, version.version_driver, version.transceiver.version_chip_fw, version.almanac_date,
            version.chip_uid[0], version.chip_uid[1], version.chip_uid[2], version.chip_uid[3], version.chip_uid[4],
            version.chip_uid[5], version.chip_uid[6], version.chip_uid[7] );
        break;
    }
    case VERSION_DEVICE_MODEM:
    {
        printf(
            "Software version: %s\nDriver version: %s\nChip version: 0x%06x\nAlmanac date: 0x%x\nChip UID: "
            "%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n",
            version.version_sw, version.version_driver, version.modem.version_chip_fw, version.almanac_date,
            version.chip_uid[0], version.chip_uid[1], version.chip_uid[2], version.chip_uid[3], version.chip_uid[4],
            version.chip_uid[5], version.chip_uid[6], version.chip_uid[7] );
        break;
    }
    }
}

void CommunicationPrintOnly::EraseDataStored( ) { return; }

void CommunicationPrintOnly::SendDataStoredToServer( ) { return; }