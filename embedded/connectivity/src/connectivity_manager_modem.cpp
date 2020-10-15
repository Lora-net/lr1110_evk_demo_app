/**
 * @file      connectivity_manager_modem.cpp
 *
 * @brief     Implementation of connectivity manager for modem.
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

#include "connectivity_manager_modem.h"
#include "system_time.h"

#define CONNECTIVITY_MANAGER_PERIOD_TEST_GPS_TIME_MS ( 1000 )

ConnectivityManagerModem::ConnectivityManagerModem( DeviceModem* device )
    : ConnectivityManagerInterface( ), _device( device ), _has_received_event( false )
{
}

ConnectivityManagerModem::~ConnectivityManagerModem( ) {}

network_connectivity_status_t ConnectivityManagerModem::Runtime( )
{
    if( this->_has_received_event == true )
    {
        this->_has_received_event = false;

        switch( this->_last_received_event.event_type )
        {
        case LR1110_MODEM_LORAWAN_EVENT_JOINED:
        {
            uint8_t adr_profile_custom[16] = { 0 };

            this->_is_joined = true;

            this->SetAdrProfile( this->_settings.adr_profile );

            lr1110_modem_stream_init( this->_device->GetRadio( ), CONNECTIVITY_MANAGER_INTERFACE_STREAM_APP_PORT,
                                      LR1110_MODEM_FILE_ENCRYPTION_DISABLE );
            return NETWORK_CONNECTIVITY_STATUS_JOIN;
        }
        case LR1110_MODEM_LORAWAN_EVENT_STREAM_DONE:
        {
            return NETWORK_CONNECTIVITY_STATUS_STREAM_DONE;
        }
        case LR1110_MODEM_LORAWAN_EVENT_TIME_UPDATED_ALC_SYNC:
        {
            if( this->_last_received_event.buffer[0] == 0x01 )
            {
                this->_is_time_sync = true;
                return NETWORK_CONNECTIVITY_STATUS_GOT_ALC_SYNC;
            }
            else
            {
                this->_is_time_sync = false;
                return NETWORK_CONNECTIVITY_STATUS_LOST_ALC_SYNC;
            }
        }
        default:
        {
            return NETWORK_CONNECTIVITY_STATUS_NO_CHANGE;
        }
        }
    }
    return NETWORK_CONNECTIVITY_STATUS_NO_CHANGE;
}

bool ConnectivityManagerModem::IsConnectable( ) const { return true; }

void ConnectivityManagerModem::Join( network_connectivity_settings_t* settings )
{
    this->_settings.region      = settings->region;
    this->_settings.adr_profile = settings->adr_profile;

    const lr1110_modem_response_code_t set_class_response_code =
        lr1110_modem_set_class( this->_device->GetRadio( ), LR1110_LORAWAN_CLASS_A );

    this->SetRegion( this->_settings.region );

    /* Set DM info field */
    lr1110_modem_dm_info_fields_t dm_info_fields;
    dm_info_fields.dm_info_field[0] = LR1110_MODEM_DM_INFO_TYPE_GNSS_ALMANAC_STATUS;
    dm_info_fields.dm_info_field[1] = LR1110_MODEM_DM_INFO_TYPE_CHARGE;
    dm_info_fields.dm_info_field[2] = LR1110_MODEM_DM_INFO_TYPE_TEMPERATURE;
    dm_info_fields.dm_info_field[3] = LR1110_MODEM_DM_INFO_TYPE_VOLTAGE;
    dm_info_fields.dm_info_length   = 4;

    const lr1110_modem_response_code_t set_dm_info_field_response_code =
        lr1110_modem_set_dm_info_field( this->_device->GetRadio( ), &dm_info_fields );

    const lr1110_modem_response_code_t set_dm_info_interval_response_code =
        lr1110_modem_set_dm_info_interval( this->_device->GetRadio( ), LR1110_MODEM_REPORTING_INTERVAL_IN_HOUR, 1 );

    lr1110_modem_alc_sync_mode_t alc_sync_mode;
    lr1110_modem_get_alc_sync_mode( this->_device->GetRadio( ), &alc_sync_mode );

    if( alc_sync_mode == LR1110_MODEM_ALC_SYNC_MODE_DISABLE )
    {
        lr1110_modem_set_alc_sync_mode( this->_device->GetRadio( ), LR1110_MODEM_ALC_SYNC_MODE_ENABLE );
    }

    lr1110_modem_join( this->_device->GetRadio( ) );
}

void ConnectivityManagerModem::Leave( )
{
    this->_is_joined = false;
    lr1110_modem_leave_network( this->_device->GetRadio( ) );
}

network_connectivity_cmd_status_t ConnectivityManagerModem::Send( uint8_t* data, uint8_t length )
{
    lr1110_modem_response_code_t rc;
    lr1110_modem_stream_status_t stream_status;

    rc = lr1110_modem_stream_status( this->_device->GetRadio( ), CONNECTIVITY_MANAGER_INTERFACE_STREAM_APP_PORT,
                                     &stream_status );

    if( rc != LR1110_MODEM_RESPONSE_CODE_OK )
    {
        return NETWORK_CONNECTIVITY_CMD_STATUS_ERROR;
    }

    if( stream_status.free < length )
    {
        return NETWORK_CONNECTIVITY_CMD_STATUS_BUFFER_TOO_SHORT;
    }

    rc = lr1110_modem_send_stream_data( this->_device->GetRadio( ), CONNECTIVITY_MANAGER_INTERFACE_STREAM_APP_PORT,
                                        data, length );

    if( rc != LR1110_MODEM_RESPONSE_CODE_OK )
    {
        return NETWORK_CONNECTIVITY_CMD_STATUS_ERROR;
    }

    return NETWORK_CONNECTIVITY_CMD_STATUS_OK;
}

void ConnectivityManagerModem::GetCurrentRegion( network_connectivity_region_t* region )
{
    lr1110_modem_regions_t region_modem;

    lr1110_modem_get_region( this->_device->GetRadio( ), &region_modem );

    switch( region_modem )
    {
    case LR1110_LORAWAN_REGION_EU868:
    {
        *region = NETWORK_CONNECTIVITY_REGION_EU868;
        break;
    }
    case LR1110_LORAWAN_REGION_US915:
    {
        *region = NETWORK_CONNECTIVITY_REGION_US915;
        break;
    }
    default:
    {
        break;
    }
    }
}

void ConnectivityManagerModem::SetRegion( network_connectivity_region_t region )
{
    switch( region )
    {
    case NETWORK_CONNECTIVITY_REGION_EU868:
    {
        lr1110_modem_set_region( this->_device->GetRadio( ), LR1110_LORAWAN_REGION_EU868 );
        break;
    }
    case NETWORK_CONNECTIVITY_REGION_US915:
    {
        lr1110_modem_set_region( this->_device->GetRadio( ), LR1110_LORAWAN_REGION_US915 );
        break;
    }
    default:
    {
        break;
    }
    }
}

void ConnectivityManagerModem::SetAdrProfile( network_connectivity_adr_profile_t profile )
{
    uint8_t adr_list[16] = { 0 };

    switch( profile )
    {
    case NETWORK_CONNECTIVITY_ADR_NETWORK_SERVER_CONTROLLED:
    {
        lr1110_modem_set_adr_profile( this->_device->GetRadio( ), LR1110_MODEM_ADR_PROFILE_NETWORK_SERVER_CONTROLLED,
                                      adr_list );
        break;
    }
    case NETWORK_CONNECTIVITY_ADR_MOBILE_LONG_RANGE:
    {
        lr1110_modem_set_adr_profile( this->_device->GetRadio( ), LR1110_MODEM_ADR_PROFILE_MOBILE_LONG_RANGE,
                                      adr_list );
        break;
    }
    case NETWORK_CONNECTIVITY_ADR_MOBILE_LOW_POWER:
    {
        lr1110_modem_set_adr_profile( this->_device->GetRadio( ), LR1110_MODEM_ADR_PROFILE_MOBILE_LOW_POWER, adr_list );
        break;
    }
    default:
    {
        break;
    }
    }
}

void ConnectivityManagerModem::ResetCommissioningToSemtechJoinServer( )
{
    // 1. Get the factory device EUI which is the chip EUI
    lr1110_modem_chip_eui_t chip_eui = { 0 };
    lr1110_modem_get_chip_eui( this->_device->GetRadio( ), chip_eui );

    // 2. Set the Device and App (Join) EUIs
    lr1110_modem_set_dev_eui( this->_device->GetRadio( ), chip_eui );
    lr1110_modem_set_join_eui( this->_device->GetRadio( ), this->SEMTECH_DEFAULT_JOIN_EUI );

    // 3. Re-compute the keys
    lr1110_modem_derive_keys( this->_device->GetRadio( ) );

    lr1110_modem_regions_t region_modem;
    lr1110_modem_regions_t region_modem_fake;
    lr1110_modem_get_region( this->_device->GetRadio( ), &region_modem );

    region_modem_fake =
        ( region_modem != LR1110_LORAWAN_REGION_EU868 ) ? LR1110_LORAWAN_REGION_EU868 : LR1110_LORAWAN_REGION_US915;
    lr1110_modem_set_region( this->_device->GetRadio( ), region_modem_fake );
    lr1110_modem_set_region( this->_device->GetRadio( ), region_modem );
}

void ConnectivityManagerModem::InterruptHandler( const InterruptionInterface* interruption )
{
    const InterruptionModem* interrupt = dynamic_cast< const InterruptionModem* >( interruption );

    if( interrupt != 0 )
    {
        this->_last_received_event = interrupt->GetEvent( );
        this->_has_received_event  = true;
    }
}
