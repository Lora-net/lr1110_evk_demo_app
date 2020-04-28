/**
 * @file      demo_wifi_interface.cpp
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

#include "demo_wifi_interface.h"

#define WIFI_DEMO_CONSUMPTION_DCDC_CORRELATION_MA ( 12 )
#define WIFI_DEMO_CONSUMPTION_DCDC_DEMODULATION_MA ( 4 )
#define WIFI_DEMO_CONSUMPTION_LDO_CORRELATION_MA ( 24 )
#define WIFI_DEMO_CONSUMPTION_LDO_DEMODULATION_MA ( 8 )

DemoWifiInterface::DemoWifiInterface( radio_t* radio, SignalingInterface* signaling )
    : DemoBase( radio, signaling ), state( DEMO_WIFI_INIT )
{
    wifi_irq = LR1110_SYSTEM_IRQ_WIFI_SCAN_DONE;
}

DemoWifiInterface::~DemoWifiInterface( ) {}

void DemoWifiInterface::Reset( )
{
    this->DemoBase::Reset( );
    this->state                          = DEMO_WIFI_INIT;
    this->results.nbrResults             = 0;
    this->results.global_consumption_uas = 0;

    uint32_t irq_to_en_dio1 = LR1110_SYSTEM_IRQ_WIFI_SCAN_DONE;
    uint32_t irq_to_en_dio2 = 0x00;
    lr1110_system_set_dio_irq_params( this->radio, irq_to_en_dio1, irq_to_en_dio2 );
}

void DemoWifiInterface::SpecificRuntime( )
{
    switch( this->state )
    {
    case DEMO_WIFI_INIT:
    {
        this->state = DEMO_WIFI_SCAN;
        lr1110_wifi_reset_cumulative_timing( this->radio );
        break;
    }

    case DEMO_WIFI_SCAN:
    {
        this->SetWaitingForInterrupt( );

        lr1110_wifi_cfg_hardware_debarker( this->radio, true );
        this->ExecuteScan( this->radio );

        this->state = DEMO_WIFI_WAIT_FOR_SCAN;
        signaling->StartCapture( );

        break;
    }

    case DEMO_WIFI_WAIT_FOR_SCAN:
    {
        if( this->InterruptHasRaised( ) )
        {
            lr1110_system_stat1_t stat1;
            lr1110_system_stat2_t stat2;
            uint32_t              irq_status;

            lr1110_system_get_status( this->radio, &stat1, &stat2, &irq_status );

            if( ( irq_status & LR1110_SYSTEM_IRQ_WIFI_SCAN_DONE ) != 0 )
            {
                this->ClearRegisteredIrqs( );
                signaling->StopCapture( );
                if( stat1.command_status != LR1110_SYSTEM_CMD_STATUS_OK )
                {
                    this->results.error      = true;
                    this->results.nbrResults = 0;
                    this->results.timings    = { 0 };
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
        this->FetchAndSaveResults( this->radio );
        lr1110_wifi_cumulative_timings_t wifi_results_timings = { 0 };

        lr1110_wifi_read_cumulative_timing( this->radio, &wifi_results_timings );

        uint32_t consumption_uas =
            DemoWifiInterface::ComputeConsumption( LR1110_SYSTEM_REG_MODE_DCDC, wifi_results_timings );
        results.timings = wifi_results_timings;
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

void DemoWifiInterface::SpecificStop( ) { signaling->StopCapture( ); }

void DemoWifiInterface::SpecificInterruptHandler( ) {}

#define CASE_STATE_STR( state ) \
    case DEMO_WIFI_##state:     \
        return ( char* ) #state;
const char* DemoWifiInterface::state2str( const demo_wifi_state_t state )
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

const demo_wifi_scan_all_results_t* DemoWifiInterface::GetResult( ) const { return &this->results; }

uint32_t DemoWifiInterface::ComputeConsumption( const lr1110_system_reg_mode_t          regMode,
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

void DemoWifiInterface::ClearRegisteredIrqs( ) const { lr1110_system_clear_irq_status( this->radio, this->wifi_irq ); }
