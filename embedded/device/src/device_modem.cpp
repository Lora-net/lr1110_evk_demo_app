/**
 * @file      device_modem.cpp
 *
 * @brief     Implementation of modem device class.
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

#include "device_modem.h"
#include "demo_configuration.h"
#include "lr1110_modem_hal.h"
#include "lr1110_modem_system.h"
#include "lr1110_modem_lorawan.h"
#include "lr1110_modem_driver_version.h"
#include "lr1110_modem_gnss.h"
#include <string.h>

DeviceModem::DeviceModem( radio_t* radio ) : DeviceInterface( radio ) {}

void DeviceModem::Init( )
{
    lr1110_modem_system_set_reg_mode( this->radio, LR1110_MODEM_SYSTEM_REG_MODE_DCDC );

    lr1110_modem_system_rf_switch_cfg_t rf_switch_setup = { 0 };
    rf_switch_setup.enable                              = DEMO_COMMON_RF_SWITCH_ENABLE;
    rf_switch_setup.standby                             = DEMO_COMMON_RF_SWITCH_STANDBY;
    rf_switch_setup.tx                                  = DEMO_COMMON_RF_SWITCH_TX;
    rf_switch_setup.tx_hp                               = DEMO_COMMON_RF_SWITCH_TX_HP;
    rf_switch_setup.rx                                  = DEMO_COMMON_RF_SWITCH_RX;
    rf_switch_setup.wifi                                = DEMO_COMMON_RF_SWITCH_WIFI;
    rf_switch_setup.gnss                                = DEMO_COMMON_RF_SWITCH_GNSS;
    lr1110_modem_system_set_dio_as_rf_switch( this->radio, &rf_switch_setup );

    lr1110_modem_system_set_tcxo_mode( this->radio, LR1110_MODEM_SYSTEM_TCXO_CTRL_3_0V, 500 );
    lr1110_modem_system_cfg_lfclk( this->radio, LR1110_MODEM_SYSTEM_LFCLK_XTAL, true );

    lr1110_modem_set_rf_output( this->radio, LR1110_MODEM_RADIO_PA_SEL_LP_HP_LF );
}

void DeviceModem::GetAlmanacAgesAndCrcOfAllSatellites( GnssHelperAlmanacDetails_t* almanac_details )
{
    for( uint8_t index_satellite = 0; index_satellite < GNSS_HELPER_NUMBER_SATELLITES_ALMANAC_READ; index_satellite++ )
    {
        almanac_details->ages_per_almanacs[index_satellite].sv_id = index_satellite;
        this->GetAlmanacAgesForSatelliteId( index_satellite,
                                            &almanac_details->ages_per_almanacs[index_satellite].almanac_age );
    }
    lr1110_modem_gnss_context_t gnss_context = { 0 };
    lr1110_modem_gnss_get_context( this->radio, &gnss_context );
    almanac_details->crc_almanac = gnss_context.global_almanac_crc;
}

void DeviceModem::GetAlmanacAgesForSatelliteId( uint8_t sv_id, uint16_t* almanac_age )
{
    uint8_t local_almanac[LR1110_MODEM_GNSS_SINGLE_ALMANAC_READ_SIZE] = { 0 };
    lr1110_modem_almanac_read_by_index( this->radio, sv_id, 1, local_almanac,
                                        LR1110_MODEM_GNSS_SINGLE_ALMANAC_READ_SIZE );
    ( *almanac_age ) = ( ( uint16_t )( local_almanac[1] ) << 0 ) + ( ( uint16_t )( local_almanac[2] ) << 8 );
}

void DeviceModem::UpdateAlmanac( const uint8_t* almanac_buffer, const uint8_t buffer_size )
{
    if( buffer_size == LR1110_MODEM_GNSS_SINGLE_ALMANAC_WRITE_SIZE )
    {
        lr1110_modem_gnss_one_chunk_almanac_update( this->radio, almanac_buffer );
    }
}

bool DeviceModem::FetchInterrupt( InterruptionInterface** interruption )
{
    lr1110_modem_event_fields_t event = {};
    lr1110_modem_get_event( this->GetRadio( ), &event );
    bool has_event = false;
    if( event.event_type == LR1110_MODEM_LORAWAN_EVENT_NO_EVENT )
    {
        has_event = false;
    }
    else
    {
        this->interruption.SetEvent( event );
        ( *interruption ) = &this->interruption;

        has_event = true;
    }
    return has_event;
}

bool DeviceModem::checkAlmanacUpdate( uint32_t expected_crc )
{
    lr1110_modem_gnss_context_t gnss_context = {};
    lr1110_modem_gnss_get_context( this->radio, &gnss_context );
    return expected_crc == gnss_context.global_almanac_crc;
}

void DeviceModem::FetchVersion( version_handler_t& version_handler )
{
    lr1110_modem_version_t lr1110_modem_version = { 0 };

    lr1110_modem_get_version( this->radio, &lr1110_modem_version );
    version_handler.device_type                   = VERSION_DEVICE_MODEM;
    version_handler.modem.version_chip_type       = lr1110_modem_version.functionality;
    version_handler.modem.version_chip_bootloader = lr1110_modem_version.bootloader;
    version_handler.modem.version_chip_fw         = lr1110_modem_version.firmware;

    lr1110_modem_get_chip_eui( this->radio, version_handler.chip_uid );
    lr1110_modem_get_dev_eui( this->radio, version_handler.dev_eui );
    lr1110_modem_get_join_eui( this->radio, version_handler.join_eui );

    uint32_t pin_raw = 0;
    lr1110_modem_get_pin( this->radio, &pin_raw );
    version_handler.pin[0] = ( uint8_t )( pin_raw >> 24 );
    version_handler.pin[1] = ( uint8_t )( pin_raw >> 16 );
    version_handler.pin[2] = ( uint8_t )( pin_raw >> 8 );
    version_handler.pin[3] = ( uint8_t )( pin_raw >> 0 );

    strcpy( version_handler.version_sw, DEMO_VERSION );
    strcpy( version_handler.version_driver, lr1110_modem_driver_version_get_version_string( ) );

    GnssHelperAlmanacDetails_t almanac_ages_crc = { 0 };
    this->GetAlmanacAgesAndCrcOfAllSatellites( &almanac_ages_crc );
    version_handler.almanac_date = almanac_ages_crc.ages_per_almanacs[0].almanac_age;
    version_handler.almanac_crc  = almanac_ages_crc.crc_almanac;
}
