/**
 * @file      communication_demo.cpp
 *
 * @brief     Implementation of the communication demo communication interface.
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
#include "communication_demo.h"
#include "communication_utils.h"
#include "system_time.h"
#include "system_uart.h"

#define COMMUNICATION_DEMO_TIMEOUT_SERIAL_RECEIVE_MS ( 5000 )
#define COMMUNICATION_DEMO_GET_RESULT_BUFFER_LENGTH ( 128 )
#define COMMUNICATION_DEMO_TMP_FORMAT_LENGTH ( 32 )
#define COMMUNICATION_DEMO_COMMAND_TOKEN_DATE "DATE"
#define COMMUNICATION_DEMO_COMMAND_TOKEN_RESULT "RESULT"
#define COMMUNICATION_DEMO_COMMAND_TOKEN_SEND "SEND"
#define COMMUNICATION_DEMO_COMMAND_TOKEN_FLUSH "FLUSH"
#define COMMUNICATION_DEMO_COMMAND_TOKEN_STORE_VERSION "VERSION"

void CommunicationDemo::Runtime( ) { return; }

void CommunicationDemo::Store( const char* fmt, ... )
{
    printf( "@" );
    va_list args;
    va_start( args, fmt );
    vprintf( fmt, args );
    va_end( args );
}

void CommunicationDemo::EraseDataStored( ) { CommunicationDemo::SendCommand( COMMUNICATION_DEMO_COMMAND_TOKEN_FLUSH ); }

void CommunicationDemo::SendDataStoredToServer( )
{
    CommunicationDemo::SendCommand( COMMUNICATION_DEMO_COMMAND_TOKEN_SEND );
}

void CommunicationDemo::Store( const demo_wifi_scan_all_results_t& wifi_results )
{
    for( uint8_t result_index = 0; result_index < wifi_results.nbrResults; result_index++ )
    {
        demo_wifi_scan_single_result_t local_result = wifi_results.results[result_index];

        this->Store(
            "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x, CHANNEL_%i, %s, %i, %i, %i, "
            "%i, "
            "%i\n",
            local_result.mac_address[0], local_result.mac_address[1], local_result.mac_address[2],
            local_result.mac_address[3], local_result.mac_address[4], local_result.mac_address[5], local_result.channel,
            CommunicationInterface::WifiTypeToStr( local_result.type ), local_result.rssi,
            wifi_results.timings.demodulation_us, wifi_results.timings.rx_capture_us,
            wifi_results.timings.rx_correlation_us, wifi_results.timings.rx_detection_us );
    }
}

void CommunicationDemo::Store( const demo_gnss_all_results_t& gnss_results, uint32_t delay_since_capture )
{
    printf( "@" );
    for( uint16_t index = 0; index < gnss_results.nav_message.size; index++ )
    {
        printf( "%02x", gnss_results.nav_message.message[index] );
    }
    printf( ", %u, %u, %u", delay_since_capture, gnss_results.timings.radio_ms, gnss_results.timings.computation_ms );
    printf( "\n" );
}

void CommunicationDemo::Store( const version_handler_t& version )
{
    switch( version.device_type )
    {
    case VERSION_DEVICE_TRANSCEIVER:
    {
        CommunicationDemo::Store( "%s:%s:%s:0x%04x:0x%x:%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n",
                                  COMMUNICATION_DEMO_COMMAND_TOKEN_STORE_VERSION, version.version_sw,
                                  version.version_driver, version.transceiver.version_chip_fw, version.almanac_date,
                                  version.chip_uid[0], version.chip_uid[1], version.chip_uid[2], version.chip_uid[3],
                                  version.chip_uid[4], version.chip_uid[5], version.chip_uid[6], version.chip_uid[7] );
        break;
    }
    case VERSION_DEVICE_MODEM:
    {
        CommunicationDemo::Store( "%s:%s:%s:0x%06x:0x%x:%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n",
                                  COMMUNICATION_DEMO_COMMAND_TOKEN_STORE_VERSION, version.version_sw,
                                  version.version_driver, version.modem.version_chip_fw, version.almanac_date,
                                  version.chip_uid[0], version.chip_uid[1], version.chip_uid[2], version.chip_uid[3],
                                  version.chip_uid[4], version.chip_uid[5], version.chip_uid[6], version.chip_uid[7] );
        break;
    }
    }
}

bool CommunicationDemo::GetDateAndApproximateLocation( uint32_t& gps_second, float& latitude, float& longitude,
                                                       float& altitude )
{
    bool                      success              = false;
    char                      data_loc_blob[64]    = { 0 };
    uint16_t                  data_loc_blob_length = 0;
    CommunicationDemoStatus_t status =
        CommunicationDemo::AskData( COMMUNICATION_DEMO_COMMAND_TOKEN_DATE, 64, data_loc_blob, data_loc_blob_length,
                                    COMMUNICATION_DEMO_TIMEOUT_SERIAL_RECEIVE_MS );
    if( status == COMMUNICATION_DEMO_STATUS_OK )
    {
        sscanf( data_loc_blob, "%u,%f,%f,%f", &gps_second, &altitude, &latitude, &longitude );
        success = true;
    }
    else
    {
        success = false;
    }
    return success;
}

bool CommunicationDemo::GetResults( float& latitude, float& longitude, float& altitude, float& accuracy,
                                    char* geo_coding, const uint8_t geo_coding_max_length )
{
    uint8_t                   count_attempt     = 0;
    const uint8_t             max_count_attempt = 1;
    CommunicationDemoStatus_t status            = COMMUNICATION_DEMO_STATUS_TIMEOUT;
    do
    {
        count_attempt++;
        status = this->AskAndParseResults( latitude, longitude, altitude, accuracy, geo_coding, geo_coding_max_length );
        this->Log( "GetResult status: 0x%x\n", status );
    } while( ( status != COMMUNICATION_DEMO_STATUS_OK ) && ( count_attempt < max_count_attempt ) );

    return ( status == COMMUNICATION_DEMO_STATUS_OK );
}

void CommunicationDemo::vLog( const char* fmt, va_list argp )
{
    printf( "# " );
    vprintf( fmt, argp );
}

bool CommunicationDemo::HasNewCommand( ) const { return false; }

CommandInterface* CommunicationDemo::FetchCommand( ) { return nullptr; }

CommunicationDemoStatus_t CommunicationDemo::ReceiveData( const uint16_t buffer_length_max, char* buffer,
                                                          uint16_t& buffer_length_received, const uint16_t timeout )
{
    CommunicationDemoStatus_t           com_status = COMMUNICATION_DEMO_STATUS_TIMEOUT;
    CommunicationUtilsReceptionStatus_t status =
        CommunicationUtilsReceiveData( buffer_length_max, buffer, buffer_length_received, timeout );
    switch( status )
    {
    case COMMUNICATION_UTILS_RECEPTION_UTILS_STATUS_OK:
    {
        com_status = COMMUNICATION_DEMO_STATUS_OK;
        break;
    }
    case COMMUNICATION_UTILS_RECEPTION_UTILS_STATUS_TIMEOUT:
    {
        com_status = COMMUNICATION_DEMO_STATUS_TIMEOUT;
        break;
    }
    case COMMUNICATION_UTILS_RECEPTION_UTILS_STATUS_OVERFLOW:
    {
        com_status = COMMUNICATION_DEMO_STATUS_OVERFLOW;
        break;
    }
    }
    return com_status;

    // uint32_t timer_count_ms      = system_time_GetTicker( );
    // int      c                   = 0;
    // uint16_t local_buffer_length = 0;
    // while( true )
    // {
    //     if( system_uart_is_readable( ) )
    //     {
    //         c                           = system_uart_receive_char( );
    //         buffer[local_buffer_length] = c;
    //         local_buffer_length++;
    //         if( c == 0 )
    //         {
    //             buffer_length_received = local_buffer_length;
    //             return COMMUNICATION_DEMO_STATUS_OK;
    //         }
    //         timer_count_ms = system_time_GetTicker( );
    //     }
    //     if( buffer_length_received >= buffer_length_max )
    //     {
    //         buffer_length_received = local_buffer_length;
    //         return COMMUNICATION_DEMO_STATUS_OVERFLOW;
    //     }
    //     if( ( system_time_GetTicker( ) - timer_count_ms ) > ( timeout ) )
    //     {
    //         buffer_length_received = local_buffer_length;
    //         return COMMUNICATION_DEMO_STATUS_TIMEOUT;
    //     }
    // }
}

CommunicationDemoStatus_t CommunicationDemo::AskData( const char* token, const uint16_t buffer_length_max, char* buffer,
                                                      uint16_t& buffer_length_received, const uint16_t timeout )
{
    buffer_length_received = 0;
    CommunicationDemo::SendCommand( token );
    system_uart_start_receiving( );
    const CommunicationDemoStatus_t status =
        CommunicationDemo::ReceiveData( buffer_length_max, buffer, buffer_length_received, timeout );
    system_uart_stop_receiving( );
    return status;
}

void CommunicationDemo::SendCommand( const char* command ) { printf( "!%s\n", command ); }

CommunicationDemoStatus_t CommunicationDemo::AskAndParseResults( float& latitude, float& longitude, float& altitude,
                                                                 float& accuracy, char* geo_coding,
                                                                 const uint8_t geo_coding_max_length )
{
    char     reception_buffer[COMMUNICATION_DEMO_GET_RESULT_BUFFER_LENGTH] = { 0 };
    char     format[COMMUNICATION_DEMO_TMP_FORMAT_LENGTH]                  = { 0 };
    uint16_t reception_buffer_size                                         = 0;

    CommunicationDemoStatus_t status =
        this->AskData( COMMUNICATION_DEMO_COMMAND_TOKEN_RESULT, COMMUNICATION_DEMO_GET_RESULT_BUFFER_LENGTH,
                       reception_buffer, reception_buffer_size, COMMUNICATION_DEMO_TIMEOUT_SERIAL_RECEIVE_MS );
    if( status == COMMUNICATION_DEMO_STATUS_OK )
    {
        snprintf( format, COMMUNICATION_DEMO_TMP_FORMAT_LENGTH, "%%f;%%f;%%f;%%f;%%%d[^\t\n]",
                  geo_coding_max_length - 1 );
        sscanf( reception_buffer, format, &latitude, &longitude, &altitude, &accuracy, geo_coding );
    }
    return status;
}