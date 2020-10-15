/**
 * @file      demo_modem_wifi.cpp
 *
 * @brief     Implementation of the Wi-Fi modem demonstration.
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

#include <string.h>
#include "demo_modem_wifi.h"
#include "lr1110_modem_system.h"
#include "lr1110_modem_wifi.h"
#include "lr1110_modem_lorawan.h"

#define WIFI_MODEM_DEMO_CONSUMPTION_DCDC_CORRELATION_MA ( 12 )
#define WIFI_MODEM_DEMO_CONSUMPTION_DCDC_DEMODULATION_MA ( 4 )
#define WIFI_MODEM_DEMO_CONSUMPTION_LDO_CORRELATION_MA ( 24 )
#define WIFI_MODEM_DEMO_CONSUMPTION_LDO_DEMODULATION_MA ( 8 )
#define WIFI_MODEM_DEMO_SCAN_ABORT_ON_TIMEOUT ( false )
#define DEMO_MODEM_WIFI_MAX_RESULTS_PER_SCAN ( 32 )

DemoModemWifi::DemoModemWifi( DeviceModem* device, SignalingInterface* signaling,
                              CommunicationInterface* communication_interface )
    : DemoModemInterface( device, signaling, communication_interface ), state( DEMO_MODEM_WIFI_INIT )
{
}

void DemoModemWifi::Reset( )
{
    this->DemoInterface::Reset( );
    this->state                          = DEMO_MODEM_WIFI_INIT;
    this->results.nbrResults             = 0;
    this->results.global_consumption_uas = 0;
}

void DemoModemWifi::SpecificRuntime( )
{
    switch( this->state )
    {
    case DEMO_MODEM_WIFI_INIT:
    {
        this->state = DEMO_MODEM_WIFI_SCAN;
        lr1110_modem_wifi_reset_cumulative_timing( this->device->GetRadio( ) );
        break;
    }

    case DEMO_MODEM_WIFI_SCAN:
    {
        this->SetWaitingForInterrupt( );

        lr1110_modem_wifi_cfg_hardware_debarker( this->device->GetRadio( ), true );
        this->ExecuteScan( this->device->GetRadio( ) );

        this->state = DEMO_MODEM_WIFI_WAIT_FOR_SCAN;
        signaling->StartCapture( );

        break;
    }

    case DEMO_MODEM_WIFI_WAIT_FOR_SCAN:
    {
        if( this->InterruptHasRaised( ) )
        {
            if( this->last_received_event.event_type == LR1110_MODEM_LORAWAN_EVENT_WIFI_SCAN_DONE )
            {
                this->FetchAndSaveResults( this->last_received_event.buffer, this->last_received_event.buffer_len );
                this->state = DEMO_MODEM_WIFI_TERMINATED;
            }
            else
            {
                this->SetWaitingForInterrupt( );
                this->state = DEMO_MODEM_WIFI_WAIT_FOR_SCAN;
            }
        }
        else
        {
            this->SetWaitingForInterrupt( );
            this->state = DEMO_MODEM_WIFI_WAIT_FOR_SCAN;
        }
        break;
    }

    case DEMO_MODEM_WIFI_TERMINATED:
    {
        this->state = DEMO_MODEM_WIFI_INIT;
        this->Terminate( );
        break;
    }
    }
}

void DemoModemWifi::SpecificStop( ) { signaling->StopCapture( ); }

const demo_wifi_scan_all_results_t* DemoModemWifi::GetResult( ) const { return &this->results; }

uint32_t DemoModemWifi::ComputeConsumption( const lr1110_modem_system_reg_mode_t          regMode,
                                            const lr1110_modem_wifi_cumulative_timings_t& timing )
{
    uint32_t consumption_uas = 0;

    switch( regMode )
    {
    case LR1110_MODEM_SYSTEM_REG_MODE_LDO:
        consumption_uas =
            ( ( timing.rx_correlation_us + timing.rx_capture_us ) * WIFI_MODEM_DEMO_CONSUMPTION_LDO_CORRELATION_MA +
              timing.demodulation_us * WIFI_MODEM_DEMO_CONSUMPTION_LDO_DEMODULATION_MA ) /
            1000;
        break;
    case LR1110_MODEM_SYSTEM_REG_MODE_DCDC:
        consumption_uas =
            ( ( timing.rx_correlation_us + timing.rx_capture_us ) * WIFI_MODEM_DEMO_CONSUMPTION_DCDC_CORRELATION_MA +
              timing.demodulation_us * WIFI_MODEM_DEMO_CONSUMPTION_DCDC_DEMODULATION_MA ) /
            1000;
        break;
    }

    return consumption_uas;
}

void DemoModemWifi::Configure( demo_wifi_settings_t& config ) { this->settings = config; }

void DemoModemWifi::ExecuteScan( radio_t* radio )
{
    lr1110_modem_wifi_passive_scan(
        this->device->GetRadio( ), DemoModemWifi::modem_wifi_scan_type_from_demo( this->settings.types ),
        this->settings.channels, DemoModemWifi::modem_wifi_mode_from_demo( this->settings.scan_mode ),
        this->settings.max_results, this->settings.nbr_retrials, this->settings.timeout,
        WIFI_MODEM_DEMO_SCAN_ABORT_ON_TIMEOUT, this->ResultFormatFromSetting( this->settings.result_type ) );
}

void DemoModemWifi::FetchAndSaveResults( const uint8_t* buffer, uint16_t buffer_length )
{
    lr1110_modem_wifi_cumulative_timings_t wifi_results_timings = { 0 };
    lr1110_modem_wifi_read_cumulative_timing( this->device->GetRadio( ), &wifi_results_timings );

    uint32_t consumption_uas =
        DemoModemWifi::ComputeConsumption( LR1110_MODEM_SYSTEM_REG_MODE_DCDC, wifi_results_timings );
    this->results.timings = DemoModemWifi::demo_wifi_timing_from_modem( wifi_results_timings );
    this->results.global_consumption_uas += consumption_uas;
    this->results.error = false;
    switch( this->settings.result_type )
    {
    case DEMO_WIFI_RESULT_TYPE_BASIC_COMPLETE:
    {
        this->ParseAndSaveBasicCompleteResults( buffer, buffer_length );
        break;
    }
    case DEMO_WIFI_RESULT_TYPE_BASIC_MAC_TYPE_CHANNEL:
    {
        this->ParseAndSaveBasicMacChannelTypeResults( buffer, buffer_length );
        break;
    }
    }
}

void DemoModemWifi::ParseAndSaveBasicCompleteResults( const uint8_t* buffer, uint16_t buffer_length )
{
    lr1110_modem_wifi_basic_complete_result_t wifi_results_mac_addr[DEMO_MODEM_WIFI_MAX_RESULTS_PER_SCAN] = { 0 };
    uint8_t                                   nb_result_parsed                                            = 0;

    lr1110_modem_wifi_read_complete_results( buffer, buffer_length, wifi_results_mac_addr, &nb_result_parsed );

    AddScanToResults( LR1110_MODEM_SYSTEM_REG_MODE_DCDC, this->results, wifi_results_mac_addr, nb_result_parsed );
}

void DemoModemWifi::ParseAndSaveBasicMacChannelTypeResults( const uint8_t* buffer, uint16_t buffer_length )
{
    lr1110_modem_wifi_basic_mac_type_channel_result_t wifi_results_mac_addr[DEMO_MODEM_WIFI_MAX_RESULTS_PER_SCAN] = {
        0
    };
    uint8_t nb_result_parsed = 0;

    lr1110_modem_wifi_read_basic_results( buffer, buffer_length, wifi_results_mac_addr, &nb_result_parsed );

    AddScanToResults( LR1110_MODEM_SYSTEM_REG_MODE_DCDC, this->results, wifi_results_mac_addr, nb_result_parsed );
}

void DemoModemWifi::AddScanToResults( const lr1110_modem_system_reg_mode_t             regMode,
                                      demo_wifi_scan_all_results_t&                    results,
                                      const lr1110_modem_wifi_basic_complete_result_t* scan_result,
                                      const uint8_t                                    nbr_results )
{
    for( uint8_t index = 0; ( index < nbr_results ) && ( results.nbrResults < DEMO_WIFI_MAX_RESULT_TOTAL ); index++ )
    {
        const lr1110_modem_wifi_basic_complete_result_t* local_basic_result = &scan_result[index];
        results.results[results.nbrResults].channel =
            lr1110_modem_extract_channel_from_info_byte( local_basic_result->channel_info_byte );

        results.results[results.nbrResults].type = DemoModemWifi::demo_wifi_types_from_modem(
            lr1110_modem_extract_signal_type_from_data_rate_info( local_basic_result->data_rate_info_byte ) );

        memcpy( results.results[results.nbrResults].mac_address, local_basic_result->mac_address,
                LR1110_WIFI_MAC_ADDRESS_LENGTH );

        results.results[results.nbrResults].rssi            = local_basic_result->rssi;
        results.results[results.nbrResults].country_code[0] = '?';
        results.results[results.nbrResults].country_code[1] = '?';
        results.nbrResults++;
    }
}

void DemoModemWifi::AddScanToResults( const lr1110_modem_system_reg_mode_t                     regMode,
                                      demo_wifi_scan_all_results_t&                            results,
                                      const lr1110_modem_wifi_basic_mac_type_channel_result_t* scan_result,
                                      const uint8_t                                            nbr_results )
{
    for( uint8_t index = 0; ( index < nbr_results ) && ( results.nbrResults < DEMO_WIFI_MAX_RESULT_TOTAL ); index++ )
    {
        const lr1110_modem_wifi_basic_mac_type_channel_result_t* local_basic_result = &scan_result[index];
        results.results[results.nbrResults].channel =
            lr1110_modem_extract_channel_from_info_byte( local_basic_result->channel_info_byte );

        results.results[results.nbrResults].type = DemoModemWifi::demo_wifi_types_from_modem(
            lr1110_modem_extract_signal_type_from_data_rate_info( local_basic_result->data_rate_info_byte ) );

        memcpy( results.results[results.nbrResults].mac_address, local_basic_result->mac_address,
                LR1110_WIFI_MAC_ADDRESS_LENGTH );

        results.results[results.nbrResults].rssi            = local_basic_result->rssi;
        results.results[results.nbrResults].country_code[0] = '?';
        results.results[results.nbrResults].country_code[1] = '?';
        results.nbrResults++;
    }
}

lr1110_modem_wifi_result_format_t DemoModemWifi::ResultFormatFromSetting( demo_wifi_result_type_t result_setting )
{
    lr1110_modem_wifi_result_format_t result_type = LR1110_MODEM_WIFI_RESULT_FORMAT_BASIC_COMPLETE;
    switch( result_setting )
    {
    case DEMO_WIFI_RESULT_TYPE_BASIC_COMPLETE:
    {
        result_type = LR1110_MODEM_WIFI_RESULT_FORMAT_BASIC_COMPLETE;
        break;
    }
    case DEMO_WIFI_RESULT_TYPE_BASIC_MAC_TYPE_CHANNEL:
    {
        result_type = LR1110_MODEM_WIFI_RESULT_FORMAT_BASIC_MAC_TYPE_CHANNEL;
        break;
    }
    }
    return result_type;
}

demo_wifi_timings_t DemoModemWifi::demo_wifi_timing_from_modem(
    const lr1110_modem_wifi_cumulative_timings_t& modem_timings )
{
    demo_wifi_timings_t demo_timing;
    demo_timing.demodulation_us   = modem_timings.demodulation_us;
    demo_timing.rx_capture_us     = modem_timings.rx_capture_us;
    demo_timing.rx_correlation_us = modem_timings.rx_correlation_us;
    demo_timing.rx_detection_us   = modem_timings.rx_detection_us;
    return demo_timing;
}

lr1110_modem_wifi_mode_t DemoModemWifi::modem_wifi_mode_from_demo( const demo_wifi_mode_t& demo_wifi_mode )
{
    lr1110_modem_wifi_mode_t modem_wifi_mode = LR1110_MODEM_WIFI_SCAN_MODE_BEACON;
    switch( demo_wifi_mode )
    {
    case DEMO_WIFI_SCAN_MODE_BEACON:
    {
        modem_wifi_mode = LR1110_MODEM_WIFI_SCAN_MODE_BEACON;
        break;
    }

    case DEMO_WIFI_SCAN_MODE_BEACON_AND_PACKET:
    {
        modem_wifi_mode = LR1110_MODEM_WIFI_SCAN_MODE_BEACON_AND_PACKET;
        break;
    }
    }
    return modem_wifi_mode;
}

lr1110_modem_wifi_signal_type_scan_t DemoModemWifi::modem_wifi_scan_type_from_demo(
    const demo_wifi_signal_type_scan_t& demo_wifi_scan_type )
{
    lr1110_modem_wifi_signal_type_scan_t modem_wifi_signal_type = LR1110_MODEM_WIFI_TYPE_SCAN_B;
    switch( demo_wifi_scan_type )
    {
    case DEMO_WIFI_SETTING_TYPE_B:
    {
        modem_wifi_signal_type = LR1110_MODEM_WIFI_TYPE_SCAN_B;
        break;
    }

    case DEMO_WIFI_SETTING_TYPE_G:
    {
        modem_wifi_signal_type = LR1110_MODEM_WIFI_TYPE_SCAN_G;
        break;
    }

    case DEMO_WIFI_SETTING_TYPE_N:
    {
        modem_wifi_signal_type = LR1110_MODEM_WIFI_TYPE_SCAN_N;
        break;
    }

    case DEMO_WIFI_SETTING_TYPE_B_G_N:
    {
        modem_wifi_signal_type = LR1110_MODEM_WIFI_TYPE_SCAN_B_G_N;
        break;
    }
    }
    return modem_wifi_signal_type;
}

demo_wifi_signal_type_t DemoModemWifi::demo_wifi_types_from_modem(
    lr1110_modem_wifi_signal_type_result_t wifi_type_result )
{
    demo_wifi_signal_type_t modem_wifi_type = DEMO_WIFI_TYPE_B;
    switch( wifi_type_result )
    {
    case LR1110_MODEM_WIFI_TYPE_RESULT_B:
    {
        modem_wifi_type = DEMO_WIFI_TYPE_B;
        break;
    }

    case LR1110_MODEM_WIFI_TYPE_RESULT_G:
    {
        modem_wifi_type = DEMO_WIFI_TYPE_G;
        break;
    }

    case LR1110_MODEM_WIFI_TYPE_RESULT_N:
    {
        modem_wifi_type = DEMO_WIFI_TYPE_N;
        break;
    }
    }
    return modem_wifi_type;
}