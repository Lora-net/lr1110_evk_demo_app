/**
 * @file      command_fetch_result.cpp
 *
 * @brief     Implementation of the HCI command to fetch results class.
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

#include "command_fetch_result.h"
#include "com_code.h"
#include "lr1110_wifi_types.h"

// WORKAROUND: #include "stm32l4xx_ll_utils.h" is needed to get the LL_mDelay
// later on
#include "stm32l4xx_ll_utils.h"

CommandFetchResult::CommandFetchResult( Hci& hci, EnvironmentInterface& environment, DemoManagerInterface& demo_holder )
    : hci( hci ), environment( environment ), demo_holder( demo_holder )
{
}

CommandFetchResult::~CommandFetchResult( ) {}

uint16_t CommandFetchResult::GetComCode( ) { return COM_CODE_FETCH_RESULT; }

bool CommandFetchResult::ConfigureFromPayload( const uint8_t* buffer, const uint16_t buffer_size )
{
    if( buffer_size != 0 )
    {
        return false;
    }
    else
    {
        return true;
    }
}

CommandEvent_t CommandFetchResult::Execute( )
{
    switch( this->demo_holder.GetType( ) )
    {
    case DEMO_TYPE_WIFI:
    {
        const demo_wifi_scan_all_results_t& wifi_result = *( demo_wifi_scan_all_results_t* ) demo_holder.GetResults( );
        const uint8_t                       n_results   = wifi_result.nbrResults;
        const uint16_t                      response_code = this->GetComCode( );
        this->hci.SendResponse( response_code, n_results );

        this->FetchWifiResults( wifi_result );
        break;
    }
    case DEMO_TYPE_GNSS_AUTONOMOUS:
    {
        const demo_gnss_all_results_t& gnss_autonomous_results =
            *( demo_gnss_all_results_t* ) demo_holder.GetResults( );
        // const uint8_t  n_results     = gnss_autonomous_results.nb_result;
        const uint16_t response_code = this->GetComCode( );
        this->hci.SendResponse( response_code, 1 );

        this->FetchAutonomousGnssResults( gnss_autonomous_results );
        break;
    }
    case DEMO_TYPE_GNSS_ASSISTED:
    {
        const demo_gnss_all_results_t& gnss_assisted_results = *( demo_gnss_all_results_t* ) demo_holder.GetResults( );
        // const uint8_t  n_results     = gnss_assisted_results.nb_result;
        const uint16_t response_code = this->GetComCode( );
        this->hci.SendResponse( response_code, 1 );

        this->FetchAssistedGnssResults( gnss_assisted_results );
        break;
    }
    default:
        break;
    }
    return COMMAND_NO_EVENT;
}

void CommandFetchResult::FetchWifiResults( const demo_wifi_scan_all_results_t& wifi_results )
{
    for( uint8_t result_index = 0; result_index < wifi_results.nbrResults; result_index++ )
    {
        demo_wifi_scan_single_result_t local_result          = wifi_results.results[result_index];
        const uint32_t                 local_detection_us    = wifi_results.timings.rx_detection_us;
        const uint32_t                 local_correlation_us  = wifi_results.timings.rx_correlation_us;
        const uint32_t                 local_capture_us      = wifi_results.timings.rx_capture_us;
        const uint32_t                 local_demodulation_us = wifi_results.timings.demodulation_us;

        const uint8_t single_wifi_result_buffer[] = {
            local_result.mac_address[0],
            local_result.mac_address[1],
            local_result.mac_address[2],
            local_result.mac_address[3],
            local_result.mac_address[4],
            local_result.mac_address[5],
            local_result.channel,
            CommandFetchResult::ConvertWifiTypeToSerial( local_result.type ),
            ( uint8_t ) local_result.rssi,
            ( uint8_t )( ( local_detection_us & 0x000000FF ) >> 0 ),
            ( uint8_t )( ( local_detection_us & 0x0000FF00 ) >> 8 ),
            ( uint8_t )( ( local_detection_us & 0x00FF0000 ) >> 16 ),
            ( uint8_t )( ( local_detection_us & 0xFF000000 ) >> 24 ),
            ( uint8_t )( ( local_correlation_us & 0x000000FF ) >> 0 ),
            ( uint8_t )( ( local_correlation_us & 0x0000FF00 ) >> 8 ),
            ( uint8_t )( ( local_correlation_us & 0x00FF0000 ) >> 16 ),
            ( uint8_t )( ( local_correlation_us & 0xFF000000 ) >> 24 ),
            ( uint8_t )( ( local_capture_us & 0x000000FF ) >> 0 ),
            ( uint8_t )( ( local_capture_us & 0x0000FF00 ) >> 8 ),
            ( uint8_t )( ( local_capture_us & 0x00FF0000 ) >> 16 ),
            ( uint8_t )( ( local_capture_us & 0xFF000000 ) >> 24 ),
            ( uint8_t )( ( local_demodulation_us & 0x000000FF ) >> 0 ),
            ( uint8_t )( ( local_demodulation_us & 0x0000FF00 ) >> 8 ),
            ( uint8_t )( ( local_demodulation_us & 0x00FF0000 ) >> 16 ),
            ( uint8_t )( ( local_demodulation_us & 0xFF000000 ) >> 24 ),
        };
        hci.SendResponse( RESP_CODE_WIFI_RESULT, single_wifi_result_buffer,
                          sizeof( single_wifi_result_buffer ) / sizeof( *single_wifi_result_buffer ) );

        // WORKAROUND: using LL_mDelay(1) fixes the bug appearing when sending >
        // 20 Wi-Fi results
        LL_mDelay( 1 );
    }
}

void CommandFetchResult::FetchAutonomousGnssResults( const demo_gnss_all_results_t& gnss_autonomous_results )
{
    this->SendGnssResult( gnss_autonomous_results, RESP_CODE_GNSS_AUTONOMOUS_RESULT );
}

void CommandFetchResult::FetchAssistedGnssResults( const demo_gnss_all_results_t& gnss_assisted_results )
{
    this->SendGnssResult( gnss_assisted_results, RESP_CODE_GNSS_ASSISTED_RESULT );
}

void CommandFetchResult::SendGnssResult( const demo_gnss_all_results_t& gnss_result, const uint16_t resp_code )
{
    const uint32_t local_measurement_delay =
        this->environment.GetLocalTimeSeconds( ) - gnss_result.local_instant_measurement;
    const uint16_t gnss_result_buffer_size = ( 3 * 4 ) + gnss_result.nav_message.size;

    uint8_t  gnss_result_buffer[512] = { 0 };
    uint16_t buffer_index            = 0;

    // 1. Four bytes for the local measurement delay
    buffer_index += CommandFetchResult::AppendValueAtIndex( gnss_result_buffer, buffer_index, local_measurement_delay );

    // 2. Four bytes for Radio timing
    buffer_index +=
        CommandFetchResult::AppendValueAtIndex( gnss_result_buffer, buffer_index, gnss_result.timings.radio_ms );

    // 3. Four bytes for Computation timing
    buffer_index +=
        CommandFetchResult::AppendValueAtIndex( gnss_result_buffer, buffer_index, gnss_result.timings.computation_ms );

    // 4. All the NAV message (size is variable)
    for( uint16_t index_nav_message = 0; index_nav_message < gnss_result.nav_message.size; index_nav_message++ )
    {
        gnss_result_buffer[buffer_index + index_nav_message] = gnss_result.nav_message.message[index_nav_message];
    }

    hci.SendResponse( resp_code, gnss_result_buffer, gnss_result_buffer_size );
}

uint8_t CommandFetchResult::AppendValueAtIndex( uint8_t* array, const uint16_t index, const uint32_t value )
{
    array[index + 0] = ( uint8_t )( ( value & 0x000000FF ) >> 0 );
    array[index + 1] = ( uint8_t )( ( value & 0x0000FF00 ) >> 8 );
    array[index + 2] = ( uint8_t )( ( value & 0x00FF0000 ) >> 16 );
    array[index + 3] = ( uint8_t )( ( value & 0xFF000000 ) >> 24 );

    return 4;
}

uint8_t CommandFetchResult::ConvertWifiTypeToSerial( const demo_wifi_signal_type_t& wifi_result_type )
{
    uint8_t value = 0;
    switch( wifi_result_type )
    {
    case DEMO_WIFI_TYPE_B:
    {
        value = 1;
        break;
    }

    case DEMO_WIFI_TYPE_G:
    {
        value = 2;
        break;
    }

    case DEMO_WIFI_TYPE_N:
    {
        value = 2;
        break;
    }
    }
    return value;
}