/**
 * @file      demo_transceiver_wifi_interface.cpp
 *
 * @brief     Implementation of the Wi-Fi demonstration interface.
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

#include "demo_transceiver_wifi_interface.h"
#include "lr1110_wifi.h"

#define WIFI_DEMO_CONSUMPTION_DCDC_CORRELATION_MA ( 12 )
#define WIFI_DEMO_CONSUMPTION_DCDC_DEMODULATION_MA ( 4 )
#define WIFI_DEMO_CONSUMPTION_LDO_CORRELATION_MA ( 24 )
#define WIFI_DEMO_CONSUMPTION_LDO_DEMODULATION_MA ( 8 )

DemoTransceiverWifiInterface::DemoTransceiverWifiInterface( DeviceTransceiver* device, SignalingInterface* signaling,
                                                            CommunicationInterface* communication_interface,
                                                            EnvironmentInterface*   environment )
    : DemoTransceiverInterface( device, signaling, communication_interface, environment ), state( DEMO_WIFI_INIT )
{
    wifi_irq = LR1110_SYSTEM_IRQ_WIFI_SCAN_DONE;
}

DemoTransceiverWifiInterface::~DemoTransceiverWifiInterface( ) {}

void DemoTransceiverWifiInterface::Reset( )
{
    this->DemoInterface::Reset( );
    this->state                          = DEMO_WIFI_INIT;
    this->results.nbrResults             = 0;
    this->results.global_consumption_uas = 0;

    uint32_t irq_to_en_dio1 = LR1110_SYSTEM_IRQ_WIFI_SCAN_DONE;
    uint32_t irq_to_en_dio2 = 0x00;
    lr1110_system_set_dio_irq_params( this->device->GetRadio( ), irq_to_en_dio1, irq_to_en_dio2 );
}

void DemoTransceiverWifiInterface::SpecificRuntime( )
{
    switch( this->state )
    {
    case DEMO_WIFI_INIT:
    {
        this->state = DEMO_WIFI_SCAN;
        lr1110_wifi_reset_cumulative_timing( this->device->GetRadio( ) );
        break;
    }

    case DEMO_WIFI_SCAN:
    {
        this->SetWaitingForInterrupt( );

        lr1110_wifi_cfg_hardware_debarker( this->device->GetRadio( ), true );
        this->ExecuteScan( this->device->GetRadio( ) );

        this->state = DEMO_WIFI_WAIT_FOR_SCAN;
        signaling->StartCapture( );

        break;
    }

    case DEMO_WIFI_WAIT_FOR_SCAN:
    {
        if( this->InterruptHasRaised( ) )
        {
            if( ( this->last_received_irq_mask & LR1110_SYSTEM_IRQ_WIFI_SCAN_DONE ) != 0 )
            {
                signaling->StopCapture( );
                if( this->last_received_stat_1.command_status != LR1110_SYSTEM_CMD_STATUS_OK )
                {
                    this->results.error      = true;
                    this->results.nbrResults = 0;
                    this->results.timings    = { };
                    this->state              = DEMO_WIFI_TERMINATED;
                }
                else
                {
                    this->state = DEMO_WIFI_GET_RESULTS;
                }
            }
            else
            {
                this->SetWaitingForInterrupt( );
                this->state = DEMO_WIFI_WAIT_FOR_SCAN;
            }
        }
        else
        {
            this->SetWaitingForInterrupt( );
            this->state = DEMO_WIFI_WAIT_FOR_SCAN;
        }
        break;
    }

    case DEMO_WIFI_GET_RESULTS:
    {
        this->FetchAndSaveResults( this->device->GetRadio( ) );
        lr1110_wifi_cumulative_timings_t wifi_results_timings = { 0 };

        lr1110_wifi_read_cumulative_timing( this->device->GetRadio( ), &wifi_results_timings );

        uint32_t consumption_uas =
            DemoTransceiverWifiInterface::ComputeConsumption( LR1110_SYSTEM_REG_MODE_DCDC, wifi_results_timings );
        results.timings = DemoTransceiverWifiInterface::demo_wifi_timing_from_transceiver( wifi_results_timings );
        results.global_consumption_uas += consumption_uas;
        results.error = false;
        this->state   = DEMO_WIFI_TERMINATED;

        break;
    }

    case DEMO_WIFI_TERMINATED:
    {
        this->state = DEMO_WIFI_INIT;
        this->Terminate( );
        break;
    }
    }
}

void DemoTransceiverWifiInterface::SpecificStop( ) { signaling->StopCapture( ); }

#define CASE_STATE_STR( state ) \
    case DEMO_WIFI_##state:     \
        return ( char* ) #state;
const char* DemoTransceiverWifiInterface::state2str( const demo_wifi_state_t state )
{
    switch( state )
    {
        CASE_STATE_STR( INIT );
        CASE_STATE_STR( SCAN );
        CASE_STATE_STR( WAIT_FOR_SCAN );
        CASE_STATE_STR( GET_RESULTS );
        CASE_STATE_STR( TERMINATED );
    default:
        return ( char* ) "Unknown state";
    }
}

const demo_wifi_scan_all_results_t* DemoTransceiverWifiInterface::GetResult( ) const { return &this->results; }

uint32_t DemoTransceiverWifiInterface::ComputeConsumption( const lr1110_system_reg_mode_t          regMode,
                                                           const lr1110_wifi_cumulative_timings_t& timing )
{
    uint32_t consumption_uas = 0;

    switch( regMode )
    {
    case LR1110_SYSTEM_REG_MODE_LDO:
        consumption_uas =
            ( ( timing.rx_correlation_us + timing.rx_capture_us ) * WIFI_DEMO_CONSUMPTION_LDO_CORRELATION_MA +
              timing.demodulation_us * WIFI_DEMO_CONSUMPTION_LDO_DEMODULATION_MA ) /
            1000;
        break;
    case LR1110_SYSTEM_REG_MODE_DCDC:
        consumption_uas =
            ( ( timing.rx_correlation_us + timing.rx_capture_us ) * WIFI_DEMO_CONSUMPTION_DCDC_CORRELATION_MA +
              timing.demodulation_us * WIFI_DEMO_CONSUMPTION_DCDC_DEMODULATION_MA ) /
            1000;
        break;
    }

    return consumption_uas;
}

lr1110_wifi_mode_t DemoTransceiverWifiInterface::transceiver_wifi_mode_from_demo(
    const demo_wifi_mode_t& demo_wifi_mode )
{
    lr1110_wifi_mode_t transceiver_wifi_mode = LR1110_WIFI_SCAN_MODE_BEACON;
    switch( demo_wifi_mode )
    {
    case DEMO_WIFI_SCAN_MODE_BEACON:
    {
        transceiver_wifi_mode = LR1110_WIFI_SCAN_MODE_BEACON;
        break;
    }

    case DEMO_WIFI_SCAN_MODE_BEACON_AND_PACKET:
    {
        transceiver_wifi_mode = LR1110_WIFI_SCAN_MODE_BEACON_AND_PKT;
        break;
    }

    default:
        break;
    }
    return transceiver_wifi_mode;
}

lr1110_wifi_signal_type_scan_t DemoTransceiverWifiInterface::transceiver_wifi_scan_type_from_demo(
    const demo_wifi_signal_type_scan_t& demo_wifi_scan_type )
{
    lr1110_wifi_signal_type_scan_t transceiver_signal_type_scan = LR1110_WIFI_TYPE_SCAN_B;
    switch( demo_wifi_scan_type )
    {
    case DEMO_WIFI_SETTING_TYPE_B:
    {
        transceiver_signal_type_scan = LR1110_WIFI_TYPE_SCAN_B;
        break;
    }
    case DEMO_WIFI_SETTING_TYPE_G:
    {
        transceiver_signal_type_scan = LR1110_WIFI_TYPE_SCAN_G;
        break;
    }
    case DEMO_WIFI_SETTING_TYPE_N:
    {
        transceiver_signal_type_scan = LR1110_WIFI_TYPE_SCAN_N;
        break;
    }
    case DEMO_WIFI_SETTING_TYPE_B_G_N:
    {
        transceiver_signal_type_scan = LR1110_WIFI_TYPE_SCAN_B_G_N;
        break;
    }
    }
    return transceiver_signal_type_scan;
}

void DemoTransceiverWifiInterface::ClearRegisteredIrqs( ) const
{
    lr1110_system_clear_irq_status( this->device->GetRadio( ), this->wifi_irq );
}

demo_wifi_timings_t DemoTransceiverWifiInterface::demo_wifi_timing_from_transceiver(
    const lr1110_wifi_cumulative_timings_t& transceiver_timings )
{
    demo_wifi_timings_t demo_timing;
    demo_timing.demodulation_us   = transceiver_timings.demodulation_us;
    demo_timing.rx_capture_us     = transceiver_timings.rx_capture_us;
    demo_timing.rx_correlation_us = transceiver_timings.rx_correlation_us;
    demo_timing.rx_detection_us   = transceiver_timings.rx_detection_us;
    return demo_timing;
}

#define WIFI_TYPE_FROM_TRANSCEIVER_CASE( var, type ) \
    case LR1110_WIFI_TYPE_RESULT_##type:             \
    {                                                \
        var = DEMO_WIFI_TYPE_##type;                 \
        break;                                       \
    }
demo_wifi_signal_type_t DemoTransceiverWifiInterface::demo_wifi_types_from_transceiver(
    lr1110_wifi_signal_type_result_t wifi_type_result )
{
    demo_wifi_signal_type_t demo_wifi_type = DEMO_WIFI_TYPE_B;
    switch( wifi_type_result )
    {
        WIFI_TYPE_FROM_TRANSCEIVER_CASE( demo_wifi_type, B )
        WIFI_TYPE_FROM_TRANSCEIVER_CASE( demo_wifi_type, G )
        WIFI_TYPE_FROM_TRANSCEIVER_CASE( demo_wifi_type, N )
    }
    return demo_wifi_type;
}
#undef WIFI_TYPE_FROM_TRANSCEIVER_CASE