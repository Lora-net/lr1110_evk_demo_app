/**
 * @file      demo_radio_per.cpp
 *
 * @brief     Implementation of the Packet Error Rate demonstration.
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

#include "demo_radio_per.h"
#include "lr1110_radio.h"

DemoRadioPer::DemoRadioPer( DeviceTransceiver* device, SignalingInterface* signaling, EnvironmentInterface* environment,
                            CommunicationInterface* communication_interface, demo_radio_per_mode_t mode )
    : DemoRadioInterface( device, signaling, communication_interface ),
      environment( environment ),
      state( DEMO_RADIO_PER_STATE_INIT ),
      has_intermediate_results( false ),
      mode( mode )
{
    this->results  = {};
    this->settings = {};
}

DemoRadioPer::~DemoRadioPer( ) {}

void DemoRadioPer::SpecificRuntime( )
{
    const uint32_t               now_ms         = this->environment->GetLocalTimeMilliseconds( );
    const demo_radio_per_state_t previous_state = this->state;

    switch( this->state )
    {
    case DEMO_RADIO_PER_STATE_INIT:
    {
        this->has_intermediate_results = false;

        this->device->ResetAndInit( );
        lr1110_radio_set_pkt_type( this->device->GetRadio( ), this->settings.pkt_type );

        switch( this->settings.pkt_type )
        {
        case LR1110_RADIO_PKT_TYPE_LORA:
        {
            lr1110_radio_set_lora_mod_params( this->device->GetRadio( ), &this->settings.modulation_lora );
            lr1110_radio_set_lora_pkt_params( this->device->GetRadio( ), &this->settings.packet_lora );
            break;
        }
        case LR1110_RADIO_PKT_TYPE_GFSK:
        {
            lr1110_radio_set_gfsk_mod_params( this->device->GetRadio( ), &this->settings.modulation_gfsk );
            lr1110_radio_set_gfsk_pkt_params( this->device->GetRadio( ), &this->settings.packet_gfsk );
            break;
        }
        }

        lr1110_radio_set_rf_freq( this->device->GetRadio( ), this->settings.rf_frequency );

        lr1110_radio_set_pa_cfg( this->device->GetRadio( ), &this->settings.pa_configuration );

        lr1110_radio_set_tx_params( this->device->GetRadio( ), this->settings.tx_power, this->settings.pa_ramp_time );

        lr1110_system_set_dio_irq_params(
            this->device->GetRadio( ),
            LR1110_SYSTEM_IRQ_TX_DONE | LR1110_SYSTEM_IRQ_RX_DONE | LR1110_SYSTEM_IRQ_TIMEOUT, 0 );
        lr1110_system_clear_irq_status( this->device->GetRadio( ), LR1110_SYSTEM_IRQ_ALL_MASK );

        switch( mode )
        {
        case DEMO_RADIO_PER_MODE_TX:
            this->state = DEMO_RADIO_PER_STATE_SEND;
            break;
        case DEMO_RADIO_PER_MODE_RX:
            this->state = DEMO_RADIO_PER_STATE_SET_RX;
            break;
        default:
            break;
        }

        this->nb_of_packets_remaining = this->settings.nb_of_packets;

        for( uint16_t i = 0; i < this->settings.payload_length; i++ )
        {
            this->buffer[i] = i;
        }

        this->last_event = now_ms;

        break;
    }
    case DEMO_RADIO_PER_STATE_SEND:
    {
        if( ( now_ms - this->last_event ) > 1000 )
        {
            this->SetWaitingForInterrupt( );
            lr1110_regmem_write_buffer8( this->device->GetRadio( ), this->buffer, this->settings.payload_length );
            this->last_event = now_ms;
            lr1110_radio_set_tx( this->device->GetRadio( ), 0x00000000 );
            this->signaling->Tx( );
            this->nb_of_packets_remaining--;
            this->state = DEMO_RADIO_PER_STATE_WAIT_FOR_TX_DONE;
        }
        break;
    }
    case DEMO_RADIO_PER_STATE_WAIT_FOR_TX_DONE:
    {
        if( this->InterruptHasRaised( ) )
        {
            lr1110_system_stat1_t stat1;
            lr1110_system_stat2_t stat2;
            uint32_t              irq_status;

            lr1110_system_get_status( this->device->GetRadio( ), &stat1, &stat2, &irq_status );
            if( irq_status & LR1110_SYSTEM_IRQ_TX_DONE )
            {
                lr1110_system_clear_irq_status( this->device->GetRadio( ), LR1110_SYSTEM_IRQ_TX_DONE );
                this->results.count_tx++;
                this->has_intermediate_results = true;
                this->state =
                    ( this->nb_of_packets_remaining != 0 ) ? DEMO_RADIO_PER_STATE_SEND : DEMO_RADIO_PER_STATE_STOP;
            }
            this->ClearRegisteredIrqs( );
        }
        break;
    }

    case DEMO_RADIO_PER_STATE_SET_RX:
    {
        lr1110_radio_set_rx( this->device->GetRadio( ), 0x00000000 );
        this->state = DEMO_RADIO_PER_STATE_WAIT_FOR_RX_DONE;
        break;
    }

    case DEMO_RADIO_PER_STATE_WAIT_FOR_RX_DONE:
    {
        if( this->InterruptHasRaised( ) )
        {
            lr1110_system_stat1_t stat1;
            lr1110_system_stat2_t stat2;
            uint32_t              irq_status;

            lr1110_system_get_status( this->device->GetRadio( ), &stat1, &stat2, &irq_status );
            if( ( irq_status & LR1110_SYSTEM_IRQ_CRC_ERROR ) || ( irq_status & LR1110_SYSTEM_IRQ_HEADER_ERROR ) )
            {
                lr1110_system_clear_irq_status( this->device->GetRadio( ), LR1110_SYSTEM_IRQ_ALL_MASK );
                this->results.count_rx_wrong_packet++;
                this->has_intermediate_results = true;
                this->state                    = DEMO_RADIO_PER_STATE_SET_RX;
            }
            else if( irq_status & LR1110_SYSTEM_IRQ_RX_DONE )
            {
                this->signaling->Rx( );
                lr1110_system_clear_irq_status( this->device->GetRadio( ), LR1110_SYSTEM_IRQ_ALL_MASK );
                this->results.count_rx_correct_packet++;
                this->has_intermediate_results = true;
                this->state                    = DEMO_RADIO_PER_STATE_SET_RX;
            }
            this->ClearRegisteredIrqs( );
        }
        break;
    }

    case DEMO_RADIO_PER_STATE_STOP:
        break;
    }

    if( this->state != previous_state )
    {
        has_intermediate_results = true;
    }
    else
    {
        has_intermediate_results = false;
    }
}

void DemoRadioPer::SpecificStop( )
{
    lr1110_system_set_standby( this->device->GetRadio( ), LR1110_SYSTEM_STANDBY_CFG_RC );

    this->results.count_tx                = 0;
    this->results.count_rx_correct_packet = 0;
    this->results.count_rx_wrong_packet   = 0;
    this->results.count_rx_timeout        = 0;

    this->state = DEMO_RADIO_PER_STATE_INIT;
}

void DemoRadioPer::LogInfo( ) const {}

void DemoRadioPer::SpecificInterruptHandler( ) {}

void DemoRadioPer::ClearRegisteredIrqs( ) const {}

bool DemoRadioPer::HasIntermediateResults( ) const { return this->has_intermediate_results; }

const demo_radio_per_results_t* DemoRadioPer::GetResult( ) const { return &this->results; }
