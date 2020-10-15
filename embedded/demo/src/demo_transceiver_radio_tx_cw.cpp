/**
 * @file      demo_transceiver_radio_tx_cw.cpp
 *
 * @brief     Implementation of the Continuous Wave Transmit demonstration.
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

#include "demo_transceiver_radio_tx_cw.h"
#include "lr1110_radio.h"

DemoTransceiverRadioTxCw::DemoTransceiverRadioTxCw( DeviceTransceiver* device, SignalingInterface* signaling,
                                                    CommunicationInterface* communication_interface,
                                                    EnvironmentInterface*   environment )
    : DemoTransceiverRadioInterface( device, signaling, communication_interface, environment ),
      state( DEMO_TX_CW_STATE_INIT )
{
    this->settings = {};
}

DemoTransceiverRadioTxCw::~DemoTransceiverRadioTxCw( ) {}

void DemoTransceiverRadioTxCw::SpecificRuntime( )
{
    switch( this->state )
    {
    case DEMO_TX_CW_STATE_INIT:
    {
        this->ConfigureRadio( );
        lr1110_radio_set_tx_cw( this->device->GetRadio( ) );
        this->signaling->StartContinuousTx( );
        this->state = DEMO_TX_CW_STATE_RUNNING;
        break;
    }
    case DEMO_TX_CW_STATE_RUNNING:
    {
        break;
    }
    }
}

void DemoTransceiverRadioTxCw::SpecificStop( )
{
    lr1110_system_set_standby( this->device->GetRadio( ), LR1110_SYSTEM_STANDBY_CFG_RC );
    this->signaling->StopContinuousTx( );
    this->state = DEMO_TX_CW_STATE_INIT;
}

void DemoTransceiverRadioTxCw::ConfigureRadio( ) const
{
    // 0. Reset the device
    this->device->Init( );
    lr1110_radio_set_pkt_type( this->device->GetRadio( ), LR1110_RADIO_PKT_TYPE_LORA );

    // 1. Set RF frequency
    lr1110_radio_set_rf_freq( this->device->GetRadio( ), this->settings.rf_frequency );

    // 2. Set PA configuration
    lr1110_radio_set_pa_cfg( this->device->GetRadio( ), &this->settings.pa_configuration );

    // 3. Set tx output power
    lr1110_radio_set_tx_params( this->device->GetRadio( ), this->settings.tx_power, this->settings.pa_ramp_time );
}

void DemoTransceiverRadioTxCw::ClearRegisteredIrqs( ) const {}