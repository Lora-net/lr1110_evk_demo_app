/**
 * @file      command_configure.cpp
 *
 * @brief     Implementation of the HCI configure command class.
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

#include "command_configure.h"
#include "com_code.h"
#include "lr1110_wifi_types.h"

CommandConfigure::CommandConfigure( radio_t* radio, Hci& hci, Demo& demo_holder )
    : CommandBase( radio, hci ), demo_holder( demo_holder )
{
    this->demo_settings.wifi_settings.is_enabled   = false;
    this->demo_settings.wifi_settings.channels     = DEMO_WIFI_CHANNELS_DEFAULT >> 1;
    this->demo_settings.wifi_settings.types        = DEMO_WIFI_TYPE_SCAN_DEFAULT;
    this->demo_settings.wifi_settings.scan_mode    = DEMO_WIFI_MODE_DEFAULT;
    this->demo_settings.wifi_settings.nbr_retrials = DEMO_WIFI_NBR_RETRIALS_DEFAULT;
    this->demo_settings.wifi_settings.max_results  = DEMO_WIFI_MAX_RESULTS_DEFAULT;
    this->demo_settings.wifi_settings.timeout      = DEMO_WIFI_TIMEOUT_IN_MS_DEFAULT;

    this->demo_settings.gnss_autonomous_settings.is_enabled    = false;
    this->demo_settings.gnss_autonomous_settings.option        = DEMO_GNSS_AUTONOMOUS_OPTION_DEFAULT;
    this->demo_settings.gnss_autonomous_settings.capture_mode  = DEMO_GNSS_AUTONOMOUS_CAPTURE_MODE_DEFAULT;
    this->demo_settings.gnss_autonomous_settings.nb_satellites = DEMO_GNSS_AUTONOMOUS_N_SATELLLITE_DEFAULT;

    this->demo_settings.gnss_assisted_settings.is_enabled    = false;
    this->demo_settings.gnss_assisted_settings.option        = DEMO_GNSS_ASSISTED_OPTION_DEFAULT;
    this->demo_settings.gnss_assisted_settings.capture_mode  = DEMO_GNSS_ASSISTED_CAPTURE_MODE_DEFAULT;
    this->demo_settings.gnss_assisted_settings.nb_satellites = DEMO_GNSS_ASSISTED_N_SATELLLITE_DEFAULT;
}

CommandConfigure::~CommandConfigure( ) {}

uint16_t CommandConfigure::GetComCode( ) { return COM_CODE_CONFIGURE; }

bool CommandConfigure::ConfigureFromPayload( const uint8_t* buffer, const uint16_t buffer_size )
{
    bool configuration_success = false;

    if( buffer_size == 21 )
    {
        // Concerning the enabliing of wifi, the bufer[0] can take value 0, 1,
        // and 2.
        // 0: it means Wi-Fi scan is disabled
        // 1: it means Wi-Fi scan is enabled with WifiScan API
        // 2: it means Wi-Fi scan is enabled with Country Code search API
        const bool               wifi_enable                        = ( buffer[0] == 0 ) ? false : true;
        const bool               wifi_use_country_code              = ( buffer[0] == 2 ) ? true : false;
        const uint16_t           wifi_channel_mask                  = buffer[1] + buffer[2] * 256;
        const uint8_t            wifi_type_mask                     = buffer[3];
        const uint8_t            wifi_nbr_retrials                  = buffer[4];
        const uint8_t            wifi_max_results                   = buffer[5];
        const uint16_t           wifi_timeout_ms                    = buffer[6] + ( buffer[7] * 256 );
        const lr1110_wifi_mode_t wifi_mode                          = ( lr1110_wifi_mode_t ) buffer[8];
        const bool               gnss_autonomous_enable             = buffer[9];
        const uint8_t            gnss_autonomous_option             = buffer[10];
        const uint8_t            gnss_autonomous_capture_mode       = buffer[11];
        const uint8_t            gnss_autonomous_nb_satellite       = buffer[12];
        const uint8_t            gnss_autonomous_antenna_selection  = buffer[13];
        const uint8_t            gnss_autonomous_constellation_mask = buffer[14];
        const bool               gnss_assisted_enable               = buffer[15];
        const uint8_t            gnss_assisted_option               = buffer[16];
        const uint8_t            gnss_assisted_capture_mode         = buffer[17];
        const uint8_t            gnss_assisted_nb_satellite         = buffer[18];
        const uint8_t            gnss_assisted_antenna_selection    = buffer[19];
        const uint8_t            gnss_assisted_constellation_mask   = buffer[20];

        const bool wifi_config_success =
            this->ConfigureWifi( wifi_enable, wifi_use_country_code, wifi_channel_mask, wifi_type_mask,
                                 wifi_nbr_retrials, wifi_max_results, wifi_timeout_ms, wifi_mode );

        const bool gnss_autonomous_config_success = this->ConfigureGnssAutonomous(
            gnss_autonomous_enable, gnss_autonomous_option, gnss_autonomous_capture_mode, gnss_autonomous_nb_satellite,
            gnss_autonomous_antenna_selection, gnss_autonomous_constellation_mask );

        const bool gnss_assisted_config_success = this->ConfigureGnssAssisted(
            gnss_assisted_enable, gnss_assisted_option, gnss_assisted_capture_mode, gnss_assisted_nb_satellite,
            gnss_assisted_antenna_selection, gnss_assisted_constellation_mask );

        configuration_success = wifi_config_success && gnss_autonomous_config_success && gnss_assisted_config_success;
    }
    else
    {
        configuration_success = false;
    }
    return configuration_success;
}

bool CommandConfigure::ConfigureWifi( const bool enable, const bool use_country_code_api, const uint16_t channel_mask,
                                      const uint8_t type_mask, const uint8_t wifi_nbr_retrials,
                                      const uint8_t wifi_max_results, const uint16_t wifi_timeout_ms,
                                      const lr1110_wifi_mode_t wifi_mode )
{
    this->demo_settings.wifi_settings.channels = ( lr1110_wifi_channel_mask_t ) channel_mask;
    demo_settings.wifi_settings.types          = ( lr1110_wifi_signal_type_scan_t ) type_mask;

    this->demo_settings.wifi_settings.is_enabled = enable && ( !use_country_code_api );
    this->demo_settings.wifi_settings.scan_mode  = wifi_mode;

    this->demo_settings.wifi_settings.nbr_retrials = wifi_nbr_retrials;
    this->demo_settings.wifi_settings.max_results  = wifi_max_results;
    this->demo_settings.wifi_settings.timeout      = wifi_timeout_ms;

    this->demo_settings.wifi_country_code_settings.channels     = ( lr1110_wifi_channel_mask_t ) channel_mask;
    this->demo_settings.wifi_country_code_settings.is_enabled   = enable && ( use_country_code_api );
    this->demo_settings.wifi_country_code_settings.nbr_retrials = wifi_nbr_retrials;
    this->demo_settings.wifi_country_code_settings.max_results  = wifi_max_results;
    this->demo_settings.wifi_country_code_settings.timeout      = wifi_timeout_ms;
    return true;
}

bool CommandConfigure::ConfigureGnssAutonomous( const bool enable, const uint8_t option, const uint8_t capture_mode,
                                                const uint8_t nb_satellite, const uint8_t antenna_selection,
                                                const uint8_t constellation_mask )
{
    bool configuration_success = true;
    configuration_success      = this->ConfigureGnss( &this->demo_settings.gnss_autonomous_settings, enable, option,
                                                 capture_mode, nb_satellite, antenna_selection, constellation_mask );
    return configuration_success;
}

bool CommandConfigure::ConfigureGnssAssisted( const bool enable, const uint8_t option, const uint8_t capture_mode,
                                              const uint8_t nb_satellite, const uint8_t antenna_selection,
                                              const uint8_t constellation_mask )
{
    bool configuration_success = true;
    configuration_success      = this->ConfigureGnss( &this->demo_settings.gnss_assisted_settings, enable, option,
                                                 capture_mode, nb_satellite, antenna_selection, constellation_mask );
    return configuration_success;
}

bool CommandConfigure::ConfigureGnss( demo_gnss_settings_t* setting, const bool enable, const uint8_t option,
                                      const uint8_t capture_mode, const uint8_t nb_satellite,
                                      const uint8_t antenna_selection, const uint8_t constellation_mask )
{
    bool configuration_success = true;
    setting->is_enabled        = enable;

    // Check option value
    switch( option )
    {
    case LR1110_GNSS_OPTION_DEFAULT:
    case LR1110_GNSS_OPTION_BEST_EFFORT:
    {
        setting->option = ( lr1110_gnss_search_mode_t ) option;
        break;
    }
    default:
        configuration_success &= false;
    }

    // Check capture mode value
    switch( capture_mode )
    {
    case LR1110_GNSS_SINGLE_SCAN_MODE:
    case LR1110_GNSS_DOUBLE_SCAN_MODE:
    {
        setting->capture_mode = ( lr1110_gnss_scan_mode_t ) capture_mode;
        break;
    }
    default:
        configuration_success &= false;
    }

    setting->nb_satellites     = nb_satellite;
    setting->antenna_selection = ( demo_gnss_antenna_selection_t ) antenna_selection;

    setting->constellation_mask = constellation_mask;

    return configuration_success;
}

bool CommandConfigure::Job( )
{
    this->demo_holder.UpdateConfigWifiScan( &this->demo_settings.wifi_settings );
    this->demo_holder.UpdateConfigWifiCountryCode( &this->demo_settings.wifi_country_code_settings );
    this->demo_holder.UpdateConfigAutonomousGnss( &this->demo_settings.gnss_autonomous_settings );
    this->demo_holder.UpdateConfigAssistedGnss( &this->demo_settings.gnss_assisted_settings );
    this->SetNoEvent( );
    return true;
}
