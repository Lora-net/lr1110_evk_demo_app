/**
 * @file      demo_transceiver_radio_ping_pong.cpp
 *
 * @brief     Implementation of the Ping-Pong demonstration.
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

#include "demo_transceiver_radio_ping_pong.h"
#include "lr1110_radio.h"

#define DEMO_PING_PONG_WAIT_MASTER_PING_TO_PING_TIMEOUT_MS ( 1000 )
#define DEMO_PING_PONG_WAIT_MASTER_PING_TO_PONG_TIMEOUT_MS ( 400 )

// Master opens Pong Rx window 5 ms before Slave actually sends the Pong
#define DEMO_PING_PONG_MASTER_WAIT_START_PONG_RX ( DEMO_PING_PONG_WAIT_MASTER_PING_TO_PONG_TIMEOUT_MS - 5 )

// Master waits in Rx state for a maximum of time before going back to Tx
#define DEMO_PING_PONG_MASTER_MAX_RX_TIMEOUT ( DEMO_PING_PONG_WAIT_MASTER_PING_TO_PING_TIMEOUT_MS - 5 )

// Slave opens Ping Rx window 5 ms before Master actually sends the Ping
#define DEMO_PING_PONG_SLAVE_WAIT_START_PING_RX ( DEMO_PING_PONG_WAIT_MASTER_PING_TO_PING_TIMEOUT_MS - 5 )

DemoTransceiverRadioPingPong::DemoTransceiverRadioPingPong( DeviceTransceiver* device, SignalingInterface* signaling,
                                                            EnvironmentInterface*   environment,
                                                            CommunicationInterface* communication_interface )
    : DemoTransceiverRadioInterface( device, signaling, communication_interface, environment ),
      mode( DEMO_PING_PONG_MODE_MASTER ),
      state( DEMO_PING_PONG_STATE_INIT ),
      last_tx_done_instant_ms( 0 ),
      last_rx_done_instant_ms( 0 ),
      radio_interrupt_mask( LR1110_SYSTEM_IRQ_TX_DONE | LR1110_SYSTEM_IRQ_RX_DONE | LR1110_SYSTEM_IRQ_TIMEOUT ),
      has_intermediate_results( false )
{
    this->results  = { };
    this->settings = { };

    for( uint16_t i = 0; i < DEMO_PING_PONG_MAX_PAYLOAD_SIZE; i++ )
    {
        this->payload_ping.content[i] = 0x00;
        this->payload_pong.content[i] = 0x01;
    }
}

DemoTransceiverRadioPingPong::~DemoTransceiverRadioPingPong( ) {}

const demo_ping_pong_results_t* DemoTransceiverRadioPingPong::GetResult( ) const { return &this->results; }

void DemoTransceiverRadioPingPong::SpecificRuntime( )
{
    const uint32_t               now_ms         = this->environment->GetLocalTimeMilliseconds( );
    const demo_ping_pong_state_t previous_state = this->state;
    switch( this->state )
    {
    case DEMO_PING_PONG_STATE_INIT:
    {
        this->results.status    = DEMO_PING_PONG_STATUS_OK;
        this->results.mode      = this->mode;
        this->payload_ping.size = this->settings.payload_length;
        this->payload_pong.size = this->settings.payload_length;

        if( this->ConfigureRadio( ) == DEMO_PING_PONG_STATUS_OK )
        {
            this->last_tx_done_instant_ms = now_ms;  // Fake the instant of last received event
            this->state                   = DEMO_PING_PONG_STATE_MASTER_WAIT_SEND_PING;
            this->communication_interface->Log( "Start as Master\n" );
        }
        else
        {
            this->state = DEMO_PING_PONG_STATE_ERROR;
        }
        lr1110_radio_reset_stats( this->device->GetRadio( ) );
        break;
    }
    case DEMO_PING_PONG_STATE_MASTER_WAIT_SEND_PING:
    {
        if( ( now_ms - this->last_tx_done_instant_ms ) > DEMO_PING_PONG_WAIT_MASTER_PING_TO_PING_TIMEOUT_MS )
        {
            this->SetWaitingForInterrupt( );
            this->StartSendMessage( );
            this->signaling->Tx( );
            this->state = DEMO_PING_PONG_STATE_MASTER_WAIT_SEND_PING_DONE;
        }
        break;
    }
    case DEMO_PING_PONG_STATE_MASTER_WAIT_SEND_PING_DONE:
    {
        if( this->InterruptHasRaised( ) )
        {
            if( this->last_received_irq_mask & LR1110_SYSTEM_IRQ_TX_DONE )
            {
                this->results.count_tx++;
                this->FetchStatisticToResults( );
                this->last_tx_done_instant_ms = this->last_irq_received_instant_ms;
                this->StartReceptionMessage( );
                this->state = DEMO_PING_PONG_STATE_MASTER_WAIT_RECEIVE_PONG;
            }
            this->ClearRegisteredIrqs( );
        }
        break;
    }
    case DEMO_PING_PONG_STATE_MASTER_WAIT_START_RECEIVE_PONG:
    {
        if( ( now_ms - this->last_tx_done_instant_ms ) > DEMO_PING_PONG_MASTER_WAIT_START_PONG_RX )
        {
            this->StartReceptionMessage( );
            this->state = DEMO_PING_PONG_STATE_MASTER_WAIT_RECEIVE_PONG;
        }
        break;
    }
    case DEMO_PING_PONG_STATE_MASTER_WAIT_RECEIVE_PONG:
    {
        if( this->InterruptHasRaised( ) )
        {
            if( this->last_received_irq_mask & LR1110_SYSTEM_IRQ_CRC_ERROR )
            {
                this->results.count_rx_wrong_packet++;
                this->communication_interface->Log( "Wrong packet: CRC error\n" );
            }
            else if( this->last_received_irq_mask & LR1110_SYSTEM_IRQ_RX_DONE )
            {
                demo_ping_pong_fetched_payload_t received_payload = { 0 };
                this->FetchPayload( &received_payload );
                this->results.last_rssi = received_payload.rssi;
                if( this->IsPongPayload( received_payload.received_payload ) )
                {
                    this->signaling->Rx( );
                    this->results.count_rx_correct_packet++;
                    this->state = DEMO_PING_PONG_STATE_MASTER_WAIT_SEND_PING;
                }
                else if( this->IsPingPayload( received_payload.received_payload ) )
                {
                    // That means there is another Master on the line.
                    // Switch this one to slave and keep going.
                    this->communication_interface->Log( "Switch to Slave\n" );
                    this->last_rx_done_instant_ms = this->last_tx_done_instant_ms;
                    this->mode                    = DEMO_PING_PONG_MODE_SLAVE;
                    this->state                   = DEMO_PING_PONG_STATE_SLAVE_WAIT_SEND_PONG;
                }
                else
                {
                    this->communication_interface->Log( "Wrong packet: not ping nor pong\n --> payload (%u bytes): ",
                                                        received_payload.received_payload.size );
                    for( uint8_t index_rx_payload = 0; index_rx_payload < received_payload.received_payload.size;
                         index_rx_payload++ )
                    {
                        this->communication_interface->Log(
                            "0x%02x ", received_payload.received_payload.content[index_rx_payload] );
                    }
                    this->communication_interface->Log( "\n" );
                    this->communication_interface->Log( "Wrong packet: not ping nor pong\n" );
                    this->state = DEMO_PING_PONG_STATE_MASTER_WAIT_SEND_PING;
                    this->results.count_rx_wrong_packet++;
                }
            }
            if( this->last_received_irq_mask & LR1110_SYSTEM_IRQ_TIMEOUT )
            {
                this->communication_interface->Log( "Master Timeout\n" );
                this->state = DEMO_PING_PONG_STATE_MASTER_WAIT_SEND_PING;
                this->results.count_rx_timeout++;
            }
            this->ClearRegisteredIrqs( );
        }
        else if( ( now_ms - this->last_tx_done_instant_ms ) > DEMO_PING_PONG_MASTER_MAX_RX_TIMEOUT )
        {
            this->EndReceptionMessage( );
            this->state = DEMO_PING_PONG_STATE_MASTER_WAIT_SEND_PING;
            this->results.count_rx_timeout++;
        }
        break;
    }

    /****************/
    /* SLAVE STATES */
    /****************/
    case DEMO_PING_PONG_STATE_SLAVE_WAIT_START_RECEIVE_PING:
    {
        if( ( now_ms - this->last_rx_done_instant_ms ) > DEMO_PING_PONG_SLAVE_WAIT_START_PING_RX )
        {
            this->StartReceptionMessage( );
            this->state = DEMO_PING_PONG_STATE_SLAVE_WAIT_RECEIVE_PING;
        }
        break;
    }
    case DEMO_PING_PONG_STATE_SLAVE_WAIT_RECEIVE_PING:
    {
        if( this->InterruptHasRaised( ) )
        {
            if( this->last_received_irq_mask & LR1110_SYSTEM_IRQ_CRC_ERROR )
            {
                this->results.count_rx_wrong_packet++;
            }
            else if( this->last_received_irq_mask & LR1110_SYSTEM_IRQ_RX_DONE )
            {
                demo_ping_pong_fetched_payload_t received_payload = { 0 };
                this->FetchPayload( &received_payload );
                this->results.last_rssi = received_payload.rssi;
                if( this->IsPingPayload( received_payload.received_payload ) )
                {
                    this->last_rx_done_instant_ms = this->last_irq_received_instant_ms;
                    this->results.count_rx_correct_packet++;
                    this->signaling->Rx( );
                    this->state = DEMO_PING_PONG_STATE_SLAVE_WAIT_SEND_PONG;
                }
                else if( this->IsPongPayload( received_payload.received_payload ) )
                {
                    // That means there is another Slave on the line.
                    // Switch this one to master and keep going.
                    this->communication_interface->Log( "Wrong payload: switch to Master\n" );
                    this->last_tx_done_instant_ms = this->last_irq_received_instant_ms;
                    this->mode                    = DEMO_PING_PONG_MODE_MASTER;
                    this->state                   = DEMO_PING_PONG_STATE_MASTER_WAIT_SEND_PING;
                }
                else
                {
                    this->results.count_rx_wrong_packet++;
                }
            }
            if( this->last_received_irq_mask & LR1110_SYSTEM_IRQ_TIMEOUT )
            {
                // In case of timeout: go back to master
                this->last_tx_done_instant_ms = this->last_irq_received_instant_ms;
                this->communication_interface->Log( "Timeout: switch to Master\n" );
                this->mode  = DEMO_PING_PONG_MODE_MASTER;
                this->state = DEMO_PING_PONG_STATE_MASTER_WAIT_SEND_PING;
                this->results.count_rx_timeout++;
            }
            this->ClearRegisteredIrqs( );
        }
        if( ( now_ms - this->last_rx_done_instant_ms ) > ( 2 * DEMO_PING_PONG_SLAVE_WAIT_START_PING_RX ) )
        {
            // In case of timeout: go back to master
            this->last_tx_done_instant_ms = this->last_irq_received_instant_ms;
            this->mode                    = DEMO_PING_PONG_MODE_MASTER;
            this->state                   = DEMO_PING_PONG_STATE_MASTER_WAIT_SEND_PING;
            this->EndReceptionMessage( );
            this->results.count_rx_timeout++;
        }
        break;
    }
    case DEMO_PING_PONG_STATE_SLAVE_WAIT_SEND_PONG:
    {
        if( ( now_ms - this->last_rx_done_instant_ms ) > DEMO_PING_PONG_WAIT_MASTER_PING_TO_PONG_TIMEOUT_MS )
        {
            this->SetWaitingForInterrupt( );
            this->StartSendMessage( );
            this->signaling->Tx( );
            this->state = DEMO_PING_PONG_STATE_SLAVE_WAIT_SEND_PONG_DONE;
        }
        break;
    }
    case DEMO_PING_PONG_STATE_SLAVE_WAIT_SEND_PONG_DONE:
    {
        if( this->InterruptHasRaised( ) )
        {
            if( this->last_received_irq_mask & LR1110_SYSTEM_IRQ_TX_DONE )
            {
                this->ClearRegisteredIrqs( );
                this->results.count_tx++;
                this->FetchStatisticToResults( );
                this->StartReceptionMessage( );
                this->state = DEMO_PING_PONG_STATE_SLAVE_WAIT_RECEIVE_PING;
            }
        }
        break;
    }
    case DEMO_PING_PONG_STATE_ERROR:
    {
        this->results.status = DEMO_PING_PONG_STATUS_ERROR;
        break;
    }
    case DEMO_PING_PONG_STATE_TERMINATED:
    {
        break;
    }
    }
    if( this->state != previous_state )
    {
        has_intermediate_results = true;
        this->results.mode       = this->mode;
        this->LogInfo( );
    }
    else
    {
        has_intermediate_results = false;
    }
}

bool DemoTransceiverRadioPingPong::HasIntermediateResults( ) const { return has_intermediate_results; }

void DemoTransceiverRadioPingPong::SpecificStop( )
{
    lr1110_system_set_standby( this->device->GetRadio( ), LR1110_SYSTEM_STANDBY_CFG_RC );

    this->results.count_tx                = 0;
    this->results.count_rx_correct_packet = 0;
    this->results.count_rx_wrong_packet   = 0;
    this->results.count_rx_timeout        = 0;

    this->state = DEMO_PING_PONG_STATE_INIT;
}

void DemoTransceiverRadioPingPong::LogInfo( ) const
{
    this->communication_interface->Log(
        "Status: %s\n"
        "Counters:\n - rx ok: %u\n - tx ok: %u\n - rx timeout: %u\n"
        " - rx wrong: %u\n"
        "Last RSSI: %i dBm\n",
        DemoTransceiverRadioPingPong::ModeToString( this->results.mode ), this->results.count_rx_correct_packet,
        this->results.count_tx, this->results.count_rx_timeout, this->results.count_rx_wrong_packet,
        this->results.last_rssi );
}

const char* DemoTransceiverRadioPingPong::ModeToString( const demo_ping_pong_mode_t mode )
{
    switch( mode )
    {
    case DEMO_PING_PONG_MODE_MASTER:
    {
        return ( const char* ) "MASTER";
    }
    case DEMO_PING_PONG_MODE_SLAVE:
    {
        return ( const char* ) "SLAVE";
    }
    default:
    {
        return ( const char* ) "MODE UNKNOWN";
    }
    }
}

demo_ping_pong_status_t DemoTransceiverRadioPingPong::ConfigureRadio( ) const
{
    // 0. Reset the device
    this->device->Init( );

    // 1. Set packet type
    lr1110_radio_set_pkt_type( this->device->GetRadio( ), this->settings.pkt_type );

    // 2. Set modulation and packet parameters
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
    default:
    {
        return DEMO_PING_PONG_STATUS_ERROR;
    }
    }

    // 3. Set RF frequency
    lr1110_radio_set_rf_freq( this->device->GetRadio( ), this->settings.rf_frequency );

    // 4. Set PA configuration
    lr1110_radio_set_pa_cfg( this->device->GetRadio( ), &this->settings.pa_configuration );

    // 5. Set tx output power
    lr1110_radio_set_tx_params( this->device->GetRadio( ), this->settings.tx_power, this->settings.pa_ramp_time );

    // 6. Enable IRQs
    lr1110_system_set_dio_irq_params( this->device->GetRadio( ), this->radio_interrupt_mask, 0 );

    return DEMO_PING_PONG_STATUS_OK;
}

void DemoTransceiverRadioPingPong::StartSendMessage( ) const
{
    switch( this->mode )
    {
    case DEMO_PING_PONG_MODE_MASTER:
    {
        DemoTransceiverRadioPingPong::TransmitPayload( this->device->GetRadio( ), &this->payload_ping,
                                                       DEMO_PING_PONG_TX_TIMEOUT_DEFAULT );
        break;
    }
    case DEMO_PING_PONG_MODE_SLAVE:
    {
        DemoTransceiverRadioPingPong::TransmitPayload( this->device->GetRadio( ), &this->payload_pong,
                                                       DEMO_PING_PONG_TX_TIMEOUT_DEFAULT );
        break;
    }
    }
}

void DemoTransceiverRadioPingPong::StartReceptionMessage( ) const
{
    lr1110_radio_set_rx( this->device->GetRadio( ), DEMO_PING_PONG_RX_TIMEOUT_DEFAULT );
}

void DemoTransceiverRadioPingPong::EndReceptionMessage( ) const
{
    lr1110_system_set_standby( this->device->GetRadio( ), LR1110_SYSTEM_STANDBY_CFG_RC );
}

void DemoTransceiverRadioPingPong::TransmitPayload( const void* radio, const demo_ping_pong_rf_payload_t* payload,
                                                    const uint32_t timeout )
{
    lr1110_regmem_write_buffer8( radio, payload->content, payload->size );
    lr1110_radio_set_tx( radio, timeout );
}

void DemoTransceiverRadioPingPong::FetchPayload( demo_ping_pong_fetched_payload_t* fetch_payload ) const
{
    lr1110_radio_rx_buffer_status_t buffer_status = { 0 };
    lr1110_radio_get_rx_buffer_status( this->device->GetRadio( ), &buffer_status );
    fetch_payload->received_payload.size = buffer_status.pld_len_in_bytes;
    lr1110_regmem_read_buffer8( this->device->GetRadio( ), fetch_payload->received_payload.content,
                                buffer_status.buffer_start_pointer, buffer_status.pld_len_in_bytes );
    switch( this->settings.pkt_type )
    {
    case LR1110_RADIO_PKT_TYPE_LORA:
    {
        lr1110_radio_pkt_status_lora_t packet_status = { 0 };
        lr1110_radio_get_lora_pkt_status( this->device->GetRadio( ), &packet_status );
        fetch_payload->rssi = packet_status.rssi_pkt_in_dbm;
        break;
    }
    case LR1110_RADIO_PKT_TYPE_GFSK:
    {
        lr1110_radio_pkt_status_gfsk_t packet_status = { 0 };
        lr1110_radio_get_gfsk_pkt_status( this->device->GetRadio( ), &packet_status );
        fetch_payload->rssi = packet_status.rssi_avg_in_dbm;
        break;
    }
    default:
    {
        fetch_payload->received_payload.size = 0;
        break;
    }
    }
}

void DemoTransceiverRadioPingPong::FetchStatisticToResults( )
{
    switch( this->settings.pkt_type )
    {
    case LR1110_RADIO_PKT_TYPE_LORA:
    {
        lr1110_radio_get_lora_stats( this->device->GetRadio( ), &this->results.statistics_lora );
        break;
    }
    case LR1110_RADIO_PKT_TYPE_GFSK:
    {
        lr1110_radio_get_gfsk_stats( this->device->GetRadio( ), &this->results.statistics_gfsk );
        break;
    }
    default:
    {
        break;
    }
    }
}

bool DemoTransceiverRadioPingPong::IsPongPayload( const demo_ping_pong_rf_payload_t& payload ) const
{
    return DemoTransceiverRadioPingPong::ArePayloadEquals( this->payload_pong, payload );
}

bool DemoTransceiverRadioPingPong::IsPingPayload( const demo_ping_pong_rf_payload_t& payload ) const
{
    return DemoTransceiverRadioPingPong::ArePayloadEquals( this->payload_ping, payload );
}

bool DemoTransceiverRadioPingPong::ArePayloadEquals( const demo_ping_pong_rf_payload_t& expected,
                                                     const demo_ping_pong_rf_payload_t& test )
{
    if( expected.size == test.size )
    {
        for( uint8_t index = 0; index < expected.size; index++ )
        {
            if( expected.content[index] != test.content[index] )
            {
                return false;
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

void DemoTransceiverRadioPingPong::ClearRegisteredIrqs( ) const
{
    lr1110_system_clear_irq_status( this->device->GetRadio( ), this->radio_interrupt_mask );
}
