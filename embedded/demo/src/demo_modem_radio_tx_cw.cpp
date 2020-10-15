/**
 * @file      demo_modem_radio_tx_cw.cpp
 *
 * @brief     Implementation of the Continuous Wave Transmit with Modem demonstration.
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

#include "demo_modem_radio_tx_cw.h"

DemoModemRadioTxCw::DemoModemRadioTxCw( DeviceModem* device, SignalingInterface* signaling,
                                        CommunicationInterface* communication_interface )
    : DemoModemInterface( device, signaling, communication_interface ),
      state( DEMO_MODEM_RADIO_TX_CONTINUOUS_WAVE_STATE_INIT )
{
    this->settings = {};
}

DemoModemRadioTxCw::~DemoModemRadioTxCw( ) {}

void DemoModemRadioTxCw::SpecificRuntime( )
{
    switch( this->state )
    {
    case DEMO_MODEM_RADIO_TX_CONTINUOUS_WAVE_STATE_INIT:
    {
        lr1110_modem_test_mode_start( this->device->GetRadio( ) );
        lr1110_modem_test_tx_cw( this->device->GetRadio( ), this->settings.frequency, this->settings.tx_power );
        this->state = DEMO_MODEM_RADIO_TX_CONTINUOUS_WAVE_STATE_RUNNING;
        this->signaling->StartContinuousTx( );
        break;
    }
    case DEMO_MODEM_RADIO_TX_CONTINUOUS_WAVE_STATE_RUNNING:
    {
        break;
    }
    }
}

void DemoModemRadioTxCw::SpecificStop( )
{
    lr1110_modem_test_exit( this->device->GetRadio( ) );
    this->signaling->StopContinuousTx( );
    this->state = DEMO_MODEM_RADIO_TX_CONTINUOUS_WAVE_STATE_INIT;
}

void DemoModemRadioTxCw::Configure( demo_radio_settings_t& settings )
{
    this->convert_radio_settings( &settings, &this->settings );
}

demo_radio_per_results_t* DemoModemRadioTxCw::GetResults( )
{
    static demo_radio_per_results_t dummy_results = {
        .count_rx_correct_packet = 0,
        .count_rx_wrong_packet   = 0,
        .count_tx                = 0,
        .count_rx_timeout        = 0,
        .last_rssi               = 0,
    };
    return &dummy_results;
}

bool DemoModemRadioTxCw::convert_radio_settings(
    const demo_radio_settings_t*       radio_settings,
    demo_modem_radio_tx_cw_settings_t* demo_radio_tx_continuous_wave_settings )
{
    bool success                                      = true;
    demo_radio_tx_continuous_wave_settings->frequency = radio_settings->rf_frequency;
    demo_radio_tx_continuous_wave_settings->tx_power  = radio_settings->tx_power;
    return success;
}