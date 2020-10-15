/**
 * @file      demo_modem_radio_tx_continuous.cpp
 *
 * @brief     Implementation of the Continuous Packet Transmit with Modem demonstration.
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

#include "demo_modem_radio_tx_continuous.h"
#include "demo_modem_radio_converters.h"

DemoModemRadioTxContinuous::DemoModemRadioTxContinuous( DeviceModem* device, SignalingInterface* signaling,
                                                        CommunicationInterface* communication_interface )
    : DemoModemInterface( device, signaling, communication_interface ),
      state( DEMO_MODEM_RADIO_TX_CONTINUOUS_STATE_INIT )
{
    this->results = {};
}

DemoModemRadioTxContinuous::~DemoModemRadioTxContinuous( ) {}

void DemoModemRadioTxContinuous::SpecificRuntime( )
{
    switch( this->state )
    {
    case DEMO_MODEM_RADIO_TX_CONTINUOUS_STATE_INIT:
    {
        // The modem must be set to correct region depending on the user frequency
        if( this->settings.frequency > 900000000 )
        {
            lr1110_modem_set_region( this->device->GetRadio( ), LR1110_LORAWAN_REGION_US915 );
        }
        else
        {
            lr1110_modem_set_region( this->device->GetRadio( ), LR1110_LORAWAN_REGION_EU868 );
        }
        lr1110_modem_test_mode_start( this->device->GetRadio( ) );
        lr1110_modem_test_tx_cont( this->device->GetRadio( ), this->settings.frequency, this->settings.tx_power,
                                   this->settings.sf, this->settings.bw, this->settings.cr,
                                   this->settings.payload_length );
        this->state = DEMO_MODEM_RADIO_TX_CONTINUOUS_STATE_RUNNING;
        break;
    }
    case DEMO_MODEM_RADIO_TX_CONTINUOUS_STATE_RUNNING:
    {
        break;
    }
    }
}

void DemoModemRadioTxContinuous::SpecificStop( )
{
    lr1110_modem_test_exit( this->device->GetRadio( ) );
    this->state = DEMO_MODEM_RADIO_TX_CONTINUOUS_STATE_INIT;
}

void DemoModemRadioTxContinuous::Configure( demo_radio_settings_t& settings )
{
    this->convert_radio_settings( &settings, &this->settings );
}

demo_radio_per_results_t* DemoModemRadioTxContinuous::GetResults( ) { return &this->results; }

bool DemoModemRadioTxContinuous::convert_radio_settings(
    const demo_radio_settings_t*         radio_settings,
    demo_radio_tx_continuous_settings_t* demo_radio_tx_continuous_settings )
{
    bool success = true;

    demo_radio_tx_continuous_settings->frequency      = radio_settings->rf_frequency;
    demo_radio_tx_continuous_settings->payload_length = radio_settings->payload_length;
    demo_radio_tx_continuous_settings->tx_power       = radio_settings->tx_power;

    switch( radio_settings->pkt_type )
    {
    case LR1110_RADIO_PKT_TYPE_GFSK:
    {
        demo_radio_tx_continuous_settings->sf = LR1110_MODEM_TST_MODE_FSK;
        // Other modulation parameters are set to dummy values
        demo_radio_tx_continuous_settings->bw = LR1110_MODEM_TST_MODE_125_KHZ;
        demo_radio_tx_continuous_settings->cr = LR1110_MODEM_TST_MODE_4_5;
        break;
    }

    case LR1110_RADIO_PKT_TYPE_LORA:
    {
        success &= DemoModemRadioConverters::convert_radio_setting_sf( radio_settings->modulation_lora.sf,
                                                                       &demo_radio_tx_continuous_settings->sf );
        success &= DemoModemRadioConverters::convert_radio_setting_bw( radio_settings->modulation_lora.bw,
                                                                       &demo_radio_tx_continuous_settings->bw );
        success &= DemoModemRadioConverters::convert_radio_setting_cr( radio_settings->modulation_lora.cr,
                                                                       &demo_radio_tx_continuous_settings->cr );
        break;
    }
    }
    return success;
}