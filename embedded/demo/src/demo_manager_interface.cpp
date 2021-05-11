/**
 * @file      demo.cpp
 *
 * @brief     Implementation of the demonstration manager interface.
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

#include "demo_manager_interface.h"

DemoManagerInterface::DemoManagerInterface( EnvironmentInterface*     environment,
                                            AntennaSelectorInterface* antenna_selector, SignalingInterface* signaling,
                                            TimerInterface* timer, CommunicationInterface* communication_interface,
                                            ConnectivityManagerInterface* connectivity_interface )
    : environment( environment ),
      antenna_selector( antenna_selector ),
      signaling( signaling ),
      timer( timer ),
      running_demo( NULL ),
      demo_type_current( DEMO_TYPE_NONE ),
      communication_interface( communication_interface ),
      connectivity_interface( connectivity_interface )
{
    this->demo_wifi_settings_default.channels              = DEMO_WIFI_CHANNELS_DEFAULT >> 1;
    this->demo_wifi_settings_default.types                 = DEMO_WIFI_TYPE_SCAN_DEFAULT;
    this->demo_wifi_settings_default.scan_mode             = DEMO_WIFI_MODE_DEFAULT;
    this->demo_wifi_settings_default.nbr_retrials          = DEMO_WIFI_NBR_RETRIALS_DEFAULT;
    this->demo_wifi_settings_default.max_results           = DEMO_WIFI_MAX_RESULTS_DEFAULT;
    this->demo_wifi_settings_default.timeout               = DEMO_WIFI_TIMEOUT_IN_MS_DEFAULT;
    this->demo_wifi_settings_default.result_type           = DEMO_WIFI_RESULT_TYPE_DEFAULT;
    this->demo_wifi_settings_default.does_abort_on_timeout = DEMO_WIFI_DOES_ABORT_ON_TIMEOUT_DEFAULT;

    this->demo_wifi_country_code_settings_default.channels              = DEMO_WIFI_CHANNELS_DEFAULT >> 1;
    this->demo_wifi_country_code_settings_default.nbr_retrials          = DEMO_WIFI_NBR_RETRIALS_DEFAULT;
    this->demo_wifi_country_code_settings_default.max_results           = DEMO_WIFI_MAX_RESULTS_DEFAULT;
    this->demo_wifi_country_code_settings_default.timeout               = DEMO_WIFI_TIMEOUT_IN_MS_DEFAULT;
    this->demo_wifi_country_code_settings_default.does_abort_on_timeout = DEMO_WIFI_DOES_ABORT_ON_TIMEOUT_DEFAULT;

    this->demo_gnss_autonomous_settings_default.option             = DEMO_GNSS_AUTONOMOUS_OPTION_DEFAULT;
    this->demo_gnss_autonomous_settings_default.capture_mode       = DEMO_GNSS_AUTONOMOUS_CAPTURE_MODE_DEFAULT;
    this->demo_gnss_autonomous_settings_default.nb_satellites      = DEMO_GNSS_AUTONOMOUS_N_SATELLLITE_DEFAULT;
    this->demo_gnss_autonomous_settings_default.antenna_selection  = DEMO_GNSS_AUTONOMOUS_ANTENNA_SELECTION_DEFAULT;
    this->demo_gnss_autonomous_settings_default.constellation_mask = DEMO_GNSS_AUTONOMOUS_CONSTELLATION_MASK_DEFAULT;

    this->demo_gnss_assisted_settings_default.option             = DEMO_GNSS_ASSISTED_OPTION_DEFAULT;
    this->demo_gnss_assisted_settings_default.capture_mode       = DEMO_GNSS_ASSISTED_CAPTURE_MODE_DEFAULT;
    this->demo_gnss_assisted_settings_default.nb_satellites      = DEMO_GNSS_ASSISTED_N_SATELLLITE_DEFAULT;
    this->demo_gnss_assisted_settings_default.antenna_selection  = DEMO_GNSS_ASSISTED_ANTENNA_SELECTION_DEFAULT;
    this->demo_gnss_assisted_settings_default.constellation_mask = DEMO_GNSS_ASSISTED_CONSTELLATION_MASK_DEFAULT;

    this->demo_radio_settings_default.rf_frequency                      = DEMO_RADIO_RF_FREQUENCY_DEFAULT;
    this->demo_radio_settings_default.tx_power                          = DEMO_RADIO_TX_POWER_DEFAULT;
    this->demo_radio_settings_default.nb_of_packets                     = DEMO_RADIO_NB_OF_PACKET_DEFAULT;
    this->demo_radio_settings_default.payload_length                    = DEMO_RADIO_PAYLOAD_LENGTH_DEFAULT;
    this->demo_radio_settings_default.pa_ramp_time                      = DEMO_RADIO_PA_RAMP_TIME_DEFAULT;
    this->demo_radio_settings_default.pa_configuration.pa_duty_cycle    = DEMO_RADIO_PA_DUTY_CYCLE_DEFAULT;
    this->demo_radio_settings_default.pa_configuration.pa_hp_sel        = DEMO_RADIO_PA_HP_SEL_DEFAULT;
    this->demo_radio_settings_default.pa_configuration.pa_reg_supply    = DEMO_RADIO_PA_REG_SUPPLY_DEFAULT;
    this->demo_radio_settings_default.pa_configuration.pa_sel           = DEMO_RADIO_PA_SEL_DEFAULT;
    this->demo_radio_settings_default.pkt_type                          = DEMO_RADIO_PACKET_TYPE_DEFAULT;
    this->demo_radio_settings_default.modulation_lora.bw                = DEMO_RADIO_LORA_BW_DEFAULT;
    this->demo_radio_settings_default.modulation_lora.sf                = DEMO_RADIO_LORA_SF_DEFAULT;
    this->demo_radio_settings_default.modulation_lora.cr                = DEMO_RADIO_LORA_CR_DEFAULT;
    this->demo_radio_settings_default.modulation_lora.ldro              = 0;
    this->demo_radio_settings_default.packet_lora.crc                   = DEMO_RADIO_LORA_CRC_DEFAULT;
    this->demo_radio_settings_default.packet_lora.header_type           = DEMO_RADIO_LORA_HDR_DEFAULT;
    this->demo_radio_settings_default.packet_lora.iq                    = DEMO_RADIO_LORA_IQ_DEFAULT;
    this->demo_radio_settings_default.packet_lora.preamble_len_in_symb  = 8;
    this->demo_radio_settings_default.modulation_gfsk.br_in_bps         = DEMO_RADIO_GFSK_BITRATE_DEFAULT;
    this->demo_radio_settings_default.modulation_gfsk.fdev_in_hz        = DEMO_RADIO_GFSK_FDEV_DEFAULT;
    this->demo_radio_settings_default.modulation_gfsk.pulse_shape       = DEMO_RADIO_GFSK_MODSHAPE_DEFAULT;
    this->demo_radio_settings_default.modulation_gfsk.bw_dsb_param      = DEMO_RADIO_GFSK_RX_BW_DEFAULT;
    this->demo_radio_settings_default.packet_gfsk.address_filtering     = DEMO_RADIO_GFSK_ADR_FILTERING_DEFAULT;
    this->demo_radio_settings_default.packet_gfsk.crc_type              = DEMO_RADIO_GFSK_CRC_DEFAULT;
    this->demo_radio_settings_default.packet_gfsk.dc_free               = DEMO_RADIO_GFSK_DC_FREE_DEFAULT;
    this->demo_radio_settings_default.packet_gfsk.header_type           = DEMO_RADIO_GFSK_HDR_DEFAULT;
    this->demo_radio_settings_default.packet_gfsk.preamble_detector     = DEMO_RADIO_GFSK_PBL_DETECT_DEFAULT;
    this->demo_radio_settings_default.packet_gfsk.preamble_len_in_bits  = DEMO_RADIO_GFSK_PBL_LENGTH_DEFAULT;
    this->demo_radio_settings_default.packet_gfsk.sync_word_len_in_bits = DEMO_RADIO_GFSK_SW_LENGTH_DEFAULT;
}

DemoManagerInterface::~DemoManagerInterface( ) {}

void DemoManagerInterface::Init( )
{
    this->SetConfigToDefault( DEMO_TYPE_WIFI );
    this->SetConfigToDefault( DEMO_TYPE_WIFI_COUNTRY_CODE );
    this->SetConfigToDefault( DEMO_TYPE_GNSS_AUTONOMOUS );
    this->SetConfigToDefault( DEMO_TYPE_GNSS_ASSISTED );
    this->SetConfigToDefault( DEMO_TYPE_RADIO_PING_PONG );
    this->SetConfigToDefault( DEMO_TYPE_RADIO_PER_TX );
    this->SetConfigToDefault( DEMO_TYPE_RADIO_PER_RX );
    this->SetConfigToDefault( DEMO_TYPE_TX_CW );
    this->SetConfigToDefault( DEMO_TYPE_TEMPERATURE );
}

void DemoManagerInterface::SetConfigToDefault( demo_type_t demo_type )
{
    switch( demo_type )
    {
    case DEMO_TYPE_WIFI:
    {
        this->demo_wifi_settings = this->demo_wifi_settings_default;
        break;
    }
    case DEMO_TYPE_GNSS_AUTONOMOUS:
    {
        this->demo_gnss_autonomous_settings = this->demo_gnss_autonomous_settings_default;
        break;
    }
    case DEMO_TYPE_WIFI_COUNTRY_CODE:
    {
        this->demo_wifi_country_code_settings.channels              = DEMO_WIFI_CHANNELS_DEFAULT >> 1;
        this->demo_wifi_country_code_settings.nbr_retrials          = DEMO_WIFI_NBR_RETRIALS_DEFAULT;
        this->demo_wifi_country_code_settings.max_results           = DEMO_WIFI_MAX_RESULTS_DEFAULT;
        this->demo_wifi_country_code_settings.timeout               = DEMO_WIFI_TIMEOUT_IN_MS_DEFAULT;
        this->demo_wifi_country_code_settings.does_abort_on_timeout = DEMO_WIFI_DOES_ABORT_ON_TIMEOUT_DEFAULT;
        break;
    }
    case DEMO_TYPE_GNSS_ASSISTED:
    {
        this->demo_gnss_assisted_settings = this->demo_gnss_assisted_settings_default;
        break;
    }
    case DEMO_TYPE_RADIO_PING_PONG:
    case DEMO_TYPE_RADIO_PER_TX:
    case DEMO_TYPE_RADIO_PER_RX:
    case DEMO_TYPE_TX_CW:
    {
        this->demo_radio_settings = this->demo_radio_settings_default;
        break;
    }
    case DEMO_TYPE_TEMPERATURE:
    default:
        break;
    }
}

void DemoManagerInterface::GetConfigDefault( demo_all_settings_t* settings )
{
    settings->wifi_settings              = this->demo_wifi_settings_default;
    settings->wifi_country_code_settings = this->demo_wifi_country_code_settings_default;
    settings->gnss_autonomous_settings   = this->demo_gnss_autonomous_settings_default;
    settings->gnss_assisted_settings     = this->demo_gnss_assisted_settings_default;
    settings->radio_settings             = this->demo_radio_settings_default;
}

void DemoManagerInterface::GetConfig( demo_all_settings_t* settings )
{
    settings->wifi_settings              = this->demo_wifi_settings;
    settings->wifi_country_code_settings = this->demo_wifi_country_code_settings;
    settings->gnss_autonomous_settings   = this->demo_gnss_autonomous_settings;
    settings->gnss_assisted_settings     = this->demo_gnss_assisted_settings;
    settings->radio_settings             = this->demo_radio_settings;
}

void DemoManagerInterface::GetConfigRadio( demo_radio_settings_t* settings ) { *settings = this->demo_radio_settings; }

void DemoManagerInterface::GetConfigWifi( demo_wifi_settings_t* settings ) { *settings = this->demo_wifi_settings; }

void DemoManagerInterface::GetConfigAutonomousGnss( demo_gnss_settings_t* settings )
{
    *settings = this->demo_gnss_autonomous_settings;
}

void DemoManagerInterface::GetConfigAssistedGnss( demo_gnss_settings_t* settings )
{
    *settings = this->demo_gnss_assisted_settings;
}

void DemoManagerInterface::UpdateConfigRadio( demo_radio_settings_t* radio_config )
{
    this->demo_radio_settings = *radio_config;
}

void DemoManagerInterface::UpdateConfigWifiScan( const demo_wifi_settings_t* wifi_config )
{
    this->demo_wifi_settings = *wifi_config;
}

void DemoManagerInterface::UpdateConfigWifiCountryCode( const demo_wifi_country_code_settings_t* wifi_config )
{
    this->demo_wifi_country_code_settings = *wifi_config;
}

void DemoManagerInterface::UpdateConfigAutonomousGnss( const demo_gnss_settings_t* gnss_autonomous_config )
{
    this->demo_gnss_autonomous_settings = *gnss_autonomous_config;
}

void DemoManagerInterface::UpdateConfigAssistedGnss( const demo_gnss_settings_t* gnss_assisted_config )
{
    this->demo_gnss_assisted_settings = *gnss_assisted_config;
}

void DemoManagerInterface::Stop( )
{
    if( this->running_demo )
    {
        this->running_demo->Stop( );
    }
}

void DemoManagerInterface::Reset( )
{
    if( this->running_demo )
    {
        this->running_demo->Reset( );
    }
}

bool DemoManagerInterface::HasIntermediateResults( ) const
{
    if( this->running_demo )
    {
        return this->running_demo->HasIntermediateResults( );
    }
    else
    {
        return false;
    }
}

void DemoManagerInterface::InterruptHandler( const InterruptionInterface* interruption )
{
    if( this->running_demo )
    {
        this->running_demo->InterruptHandler( interruption );
    }
}

demo_status_t DemoManagerInterface::Runtime( ) { return this->running_demo->Runtime( ); }

demo_type_t DemoManagerInterface::GetType( ) { return this->demo_type_current; }
