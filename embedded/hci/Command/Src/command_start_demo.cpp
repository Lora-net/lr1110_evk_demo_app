/**
 * @file      command_start_demo.cpp
 *
 * @brief     Implementation of the HCI command to start demo class.
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

#include "command_start_demo.h"
#include "com_code.h"

CommandStartDemo::CommandStartDemo( DeviceInterface* device, Hci& hci, DemoManagerInterface& demo_holder )
    : CommandBase( device, hci ), demo_id_to_start( COMMAND_BASE_NO_DEMO ), demo_holder( demo_holder )
{
    this->demo_settings.wifi_settings.channels     = DEMO_WIFI_CHANNELS_DEFAULT >> 1;
    this->demo_settings.wifi_settings.types        = DEMO_WIFI_TYPE_SCAN_DEFAULT;
    this->demo_settings.wifi_settings.scan_mode    = DEMO_WIFI_MODE_DEFAULT;
    this->demo_settings.wifi_settings.nbr_retrials = DEMO_WIFI_NBR_RETRIALS_DEFAULT;
    this->demo_settings.wifi_settings.max_results  = DEMO_WIFI_MAX_RESULTS_DEFAULT;
    this->demo_settings.wifi_settings.timeout      = DEMO_WIFI_TIMEOUT_IN_MS_DEFAULT;
    this->demo_settings.wifi_settings.result_type  = DEMO_WIFI_RESULT_TYPE_DEFAULT;

    this->demo_settings.gnss_autonomous_settings.option        = DEMO_GNSS_AUTONOMOUS_OPTION_DEFAULT;
    this->demo_settings.gnss_autonomous_settings.capture_mode  = DEMO_GNSS_AUTONOMOUS_CAPTURE_MODE_DEFAULT;
    this->demo_settings.gnss_autonomous_settings.nb_satellites = DEMO_GNSS_AUTONOMOUS_N_SATELLLITE_DEFAULT;

    this->demo_settings.gnss_assisted_settings.option        = DEMO_GNSS_ASSISTED_OPTION_DEFAULT;
    this->demo_settings.gnss_assisted_settings.capture_mode  = DEMO_GNSS_ASSISTED_CAPTURE_MODE_DEFAULT;
    this->demo_settings.gnss_assisted_settings.nb_satellites = DEMO_GNSS_ASSISTED_N_SATELLLITE_DEFAULT;
}

CommandStartDemo::~CommandStartDemo( ) {}

uint16_t CommandStartDemo::GetComCode( ) { return COM_CODE_START; }

bool CommandStartDemo::ConfigureFromPayload( const uint8_t* buffer, const uint16_t buffer_size )
{
    if( buffer_size == 0 )
    {
        return false;
    }

    bool                      success            = false;
    const CommandBaseDemoId_t demo_id            = ( CommandBaseDemoId_t ) buffer[0];
    const uint8_t*            config_buffer      = buffer + 1;
    const uint16_t            config_buffer_size = buffer_size - 1;
    switch( demo_id )
    {
    case COMMAND_BASE_DEMO_WIFI_SCAN:
    {
        success = this->ConfigureWifiScan( config_buffer, config_buffer_size );
        break;
    }

    case COMMAND_BASE_DEMO_WIFI_COUNTRY_CODE:
    {
        success = this->ConfigureWifiCountryCode( config_buffer, config_buffer_size );
        break;
    }

    case COMMAND_BASE_DEMO_GNSS_AUTONOMOUS:
    {
        success = this->ConfigureGnssAutonomous( config_buffer, config_buffer_size );
        break;
    }

    case COMMAND_BASE_DEMO_GNSS_ASSISTED:
    {
        success = this->ConfigureGnssAssisted( config_buffer, config_buffer_size );
        break;
    }

    default:
    {
        success = false;
    }
    }

    if( success == true )
    {
        this->demo_id_to_start = demo_id;
    }
    else
    {
        this->demo_id_to_start = COMMAND_BASE_NO_DEMO;
    }
    return success;
}

bool CommandStartDemo::ConfigureWifiScan( const uint8_t* buffer, const uint16_t buffer_size )
{
    bool success = false;
    if( buffer_size == 9 )
    {
        const uint16_t         wifi_channel_mask     = buffer[0] + buffer[1] * 256;
        const uint8_t          wifi_type_mask        = buffer[2];
        const uint8_t          wifi_nbr_retrials     = buffer[3];
        const uint8_t          wifi_max_results      = buffer[4];
        const uint16_t         wifi_timeout_ms       = buffer[5] + ( buffer[6] * 256 );
        const demo_wifi_mode_t wifi_mode             = CommandStartDemo::wifi_mode_from_value( buffer[7] );
        const bool             wifi_abort_on_timeout = ( buffer[8] == 0x01 ) ? true : false;

        this->demo_settings.wifi_settings.channels = ( demo_wifi_channel_mask_t ) wifi_channel_mask;
        this->demo_settings.wifi_settings.types    = CommandStartDemo::wifi_signal_type_scan_from_val( wifi_type_mask );
        this->demo_settings.wifi_settings.scan_mode             = wifi_mode;
        this->demo_settings.wifi_settings.nbr_retrials          = wifi_nbr_retrials;
        this->demo_settings.wifi_settings.max_results           = wifi_max_results;
        this->demo_settings.wifi_settings.timeout               = wifi_timeout_ms;
        this->demo_settings.wifi_settings.result_type           = DEMO_WIFI_RESULT_TYPE_DEFAULT;
        this->demo_settings.wifi_settings.does_abort_on_timeout = wifi_abort_on_timeout;
        success                                                 = true;
    }
    else
    {
        success = false;
    }
    return success;
}

bool CommandStartDemo::ConfigureWifiCountryCode( const uint8_t* buffer, const uint16_t buffer_size )
{
    bool success = false;
    if( buffer_size == 7 )
    {
        const uint16_t wifi_channel_mask     = buffer[0] + buffer[1] * 256;
        const uint8_t  wifi_nbr_retrials     = buffer[2];
        const uint8_t  wifi_max_results      = buffer[3];
        const uint16_t wifi_timeout_ms       = buffer[4] + ( buffer[5] * 256 );
        const bool     wifi_abort_on_timeout = ( buffer[6] == 0x01 ) ? true : false;

        this->demo_settings.wifi_country_code_settings.channels     = ( lr1110_wifi_channel_mask_t ) wifi_channel_mask;
        this->demo_settings.wifi_country_code_settings.nbr_retrials = wifi_nbr_retrials;
        this->demo_settings.wifi_country_code_settings.max_results  = wifi_max_results;
        this->demo_settings.wifi_country_code_settings.timeout      = wifi_timeout_ms;
        this->demo_settings.wifi_country_code_settings.does_abort_on_timeout = wifi_abort_on_timeout;
        success                                                              = true;
    }
    else
    {
        success = false;
    }
    return success;
}

bool CommandStartDemo::ConfigureGnssAutonomous( const uint8_t* buffer, const uint16_t buffer_size )
{
    const bool success = this->ConfigureGnss( &this->demo_settings.gnss_autonomous_settings, buffer, buffer_size );
    return success;
}

bool CommandStartDemo::ConfigureGnssAssisted( const uint8_t* buffer, const uint16_t buffer_size )
{
    const bool success = this->ConfigureGnss( &this->demo_settings.gnss_assisted_settings, buffer, buffer_size );
    return success;
}

bool CommandStartDemo::ConfigureGnss( demo_gnss_settings_t* gnss_setting, const uint8_t* buffer,
                                      const uint16_t buffer_size )
{
    bool success = true;  // Keep it init to true as the implementation sets it to false as soon as a parameter fails
    if( buffer_size == 5 )
    {
        const uint8_t gnss_option             = buffer[0];
        const uint8_t gnss_capture_mode       = buffer[1];
        const uint8_t gnss_nb_satellite       = buffer[2];
        const uint8_t gnss_antenna_selection  = buffer[3];
        const uint8_t gnss_constellation_mask = buffer[4];

        // Check option value
        switch( gnss_option )
        {
        case DEMO_GNSS_OPTION_DEFAULT:
        case DEMO_GNSS_OPTION_BEST_EFFORT:
        {
            gnss_setting->option = ( demo_gnss_search_mode_t ) gnss_option;
            break;
        }
        default:
            success &= false;
        }

        // Check capture mode value
        switch( gnss_capture_mode )
        {
        case DEMO_GNSS_SINGLE_SCAN_MODE:
        case DEMO_GNSS_DOUBLE_SCAN_MODE:
        {
            gnss_setting->capture_mode = ( demo_gnss_scan_mode_t ) gnss_capture_mode;
            break;
        }
        default:
            success &= false;
        }

        gnss_setting->nb_satellites      = gnss_nb_satellite;
        gnss_setting->antenna_selection  = ( demo_gnss_antenna_selection_t ) gnss_antenna_selection;
        gnss_setting->constellation_mask = gnss_constellation_mask;
    }
    else
    {
        success = false;
    }
    return success;
}

demo_wifi_mode_t CommandStartDemo::wifi_mode_from_value( const uint8_t& value )
{
    demo_wifi_mode_t wifi_mode = DEMO_WIFI_SCAN_MODE_BEACON;
    switch( value )
    {
    case 1:
    {
        wifi_mode = DEMO_WIFI_SCAN_MODE_BEACON;
        break;
    }

    case 2:
    {
        wifi_mode = DEMO_WIFI_SCAN_MODE_BEACON_AND_PACKET;
        break;
    }
    }
    return wifi_mode;
}

demo_wifi_signal_type_scan_t CommandStartDemo::wifi_signal_type_scan_from_val( const uint8_t& val )
{
    demo_wifi_signal_type_scan_t wifi_type = DEMO_WIFI_SETTING_TYPE_B;
    switch( val )
    {
    case 1:
    {
        wifi_type = DEMO_WIFI_SETTING_TYPE_B;
        break;
    }
    case 2:
    {
        wifi_type = DEMO_WIFI_SETTING_TYPE_G;
        break;
    }
    case 3:
    {
        wifi_type = DEMO_WIFI_SETTING_TYPE_B_G_N;
        break;
    }
    }
    return wifi_type;
}

bool CommandStartDemo::Job( )
{
    bool success = false;
    switch( this->demo_id_to_start )
    {
    case COMMAND_BASE_DEMO_WIFI_SCAN:
    {
        this->demo_holder.UpdateConfigWifiScan( &this->demo_settings.wifi_settings );
        success = true;
        break;
    }

    case COMMAND_BASE_DEMO_WIFI_COUNTRY_CODE:
    {
        this->demo_holder.UpdateConfigWifiCountryCode( &this->demo_settings.wifi_country_code_settings );
        success = true;
        break;
    }

    case COMMAND_BASE_DEMO_GNSS_AUTONOMOUS:
    {
        this->demo_holder.UpdateConfigAutonomousGnss( &this->demo_settings.gnss_autonomous_settings );
        success = true;
        break;
    }

    case COMMAND_BASE_DEMO_GNSS_ASSISTED:
    {
        this->demo_holder.UpdateConfigAssistedGnss( &this->demo_settings.gnss_assisted_settings );
        success = true;
        break;
    }
    default:
    {
        // The demo id to start is unknown. Reset it to NO_DEMO and indicate failure of the job
        this->demo_id_to_start = COMMAND_BASE_NO_DEMO;
        success                = false;
        break;
    }
    }
    this->SetEventStartDemo( this->demo_id_to_start );
    return success;
}