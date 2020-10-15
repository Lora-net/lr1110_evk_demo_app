/**
 * @file      device_transceiver.cpp
 *
 * @brief     Implementation of transceiver device class.
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

#include "device_transceiver.h"
#include "lr1110_system.h"
#include "lr1110_driver_version.h"
#include "lr1110_gnss.h"
#include "demo_configuration.h"
#include <string.h>

DeviceTransceiver::DeviceTransceiver( radio_t* radio ) : DeviceInterface( radio ) {}

void DeviceTransceiver::Init( )
{
    lr1110_system_set_reg_mode( this->radio, LR1110_SYSTEM_REG_MODE_DCDC );

    lr1110_system_rfswitch_cfg_t rf_switch_setup = { 0 };
    rf_switch_setup.enable                       = DEMO_COMMON_RF_SWITCH_ENABLE;
    rf_switch_setup.standby                      = DEMO_COMMON_RF_SWITCH_STANDBY;
    rf_switch_setup.tx                           = DEMO_COMMON_RF_SWITCH_TX;
    rf_switch_setup.tx_hp                        = DEMO_COMMON_RF_SWITCH_TX_HP;
    rf_switch_setup.rx                           = DEMO_COMMON_RF_SWITCH_RX;
    rf_switch_setup.wifi                         = DEMO_COMMON_RF_SWITCH_WIFI;
    rf_switch_setup.gnss                         = DEMO_COMMON_RF_SWITCH_GNSS;
    lr1110_system_set_dio_as_rf_switch( this->radio, &rf_switch_setup );

    lr1110_system_set_tcxo_mode( this->radio, LR1110_SYSTEM_TCXO_CTRL_3_0V, 500 );
    lr1110_system_cfg_lfclk( this->radio, LR1110_SYSTEM_LFCLK_XTAL, true );
    lr1110_system_clear_errors( this->radio );
    lr1110_system_calibrate( this->radio, 0x3F );

    uint16_t errors;
    lr1110_system_get_errors( this->radio, &errors );
    lr1110_system_clear_errors( this->radio );
    lr1110_system_clear_irq_status( this->radio, LR1110_SYSTEM_IRQ_ALL_MASK );
}

void DeviceTransceiver::GetAlmanacAgesAndCrcOfAllSatellites( GnssHelperAlmanacDetails_t* almanac_details )
{
    lr1110_gnss_almanac_full_read_bytestream_t almanac_bytestream = { 0 };
    lr1110_gnss_read_almanac( this->radio, almanac_bytestream );
    uint16_t index_bytestream = 0;
    for( uint8_t index_satellite = 0; ( index_bytestream < ( LR1110_GNSS_FULL_ALMANAC_READ_BUFFER_SIZE - 4 ) ) &&
                                      ( index_satellite < GNSS_HELPER_NUMBER_SATELLITES_ALMANAC_READ );
         index_bytestream += 22, index_satellite++ )
    {
        almanac_details->ages_per_almanacs[index_satellite].sv_id = almanac_bytestream[index_bytestream];
        almanac_details->ages_per_almanacs[index_satellite].almanac_age =
            ( almanac_bytestream[index_bytestream + 1] ) + ( almanac_bytestream[index_bytestream + 2] << 8 );
    }
    almanac_details->crc_almanac = almanac_bytestream[GNSS_HELPER_NUMBER_SATELLITES_ALMANAC_READ - 1] +
                                   ( almanac_bytestream[GNSS_HELPER_NUMBER_SATELLITES_ALMANAC_READ - 2] << 8 ) +
                                   ( almanac_bytestream[GNSS_HELPER_NUMBER_SATELLITES_ALMANAC_READ - 3] << 16 ) +
                                   ( almanac_bytestream[GNSS_HELPER_NUMBER_SATELLITES_ALMANAC_READ - 4] << 24 );
}

void DeviceTransceiver::GetAlmanacAgesForSatelliteId( uint8_t sv_id, uint16_t* almanac_age )
{
    lr1110_gnss_get_almanac_age_for_satellite( this->radio, sv_id, almanac_age );
}

void DeviceTransceiver::UpdateAlmanac( const uint8_t* almanac_buffer, const uint8_t buffer_size )
{
    if( buffer_size == LR1110_GNSS_SINGLE_ALMANAC_WRITE_SIZE )
    {
        lr1110_gnss_one_satellite_almanac_update( this->radio, almanac_buffer );
    }
}

bool DeviceTransceiver::FetchInterrupt( InterruptionInterface** interruption )
{
    // For the transceiver, the interrupts are all fetched at once. So the plan it to read them all from the chip, then
    // to generate an interruption object with the interruption register.
    // After that, the next call to FetchInterrupt must return false immediatly.

    uint32_t              interruption_mask = 0;
    lr1110_system_stat1_t stat1             = {};
    lr1110_system_stat2_t stat2             = {};

    lr1110_system_get_status( this->radio, &stat1, &stat2, &interruption_mask );
    lr1110_system_clear_irq_status( this->GetRadio( ), interruption_mask );

    bool has_interrupt = false;
    if( interruption_mask == LR1110_SYSTEM_IRQ_NONE )
    {
        // There is no more interrupt return false
        has_interrupt = false;
    }
    else
    {
        this->last_interrupt.set_irq_mask( interruption_mask );
        this->last_interrupt.set_stat_1( stat1 );
        this->last_interrupt.set_stat_2( stat2 );
        has_interrupt = true;
    }

    ( *interruption ) = &this->last_interrupt;
    return has_interrupt;
}

bool DeviceTransceiver::checkAlmanacUpdate( uint32_t expected_crc )
{
    bool     update_success = false;
    uint16_t result_size    = 0;
    lr1110_gnss_get_result_size( this->radio, &result_size );

    if( result_size == 2 )
    {
        uint8_t result_buffer[2] = { 0 };
        lr1110_gnss_read_results( this->radio, result_buffer, result_size );
        if( ( result_buffer[0] == 0 ) && ( result_buffer[1] == 0 ) )
        {
            uint32_t actual_crc = 0;
            lr1110_gnss_get_almanac_crc( this->radio, &actual_crc );
            update_success = ( expected_crc == actual_crc );
        }
        else
        {
            update_success = false;
        }
    }
    else
    {
        update_success = false;
    }
    return update_success;
}

void DeviceTransceiver::FetchVersion( version_handler_t& version_handler )
{
    lr1110_system_version_t lr1110_version = { 0 };

    lr1110_system_get_version( this->radio, &lr1110_version );
    version_handler.device_type                   = VERSION_DEVICE_TRANSCEIVER;
    version_handler.transceiver.version_chip_type = lr1110_version.type;
    version_handler.transceiver.version_chip_hw   = lr1110_version.hw;
    version_handler.transceiver.version_chip_fw   = lr1110_version.fw;

    lr1110_system_read_uid( this->radio, version_handler.chip_uid );
    lr1110_system_read_uid( this->radio, version_handler.dev_eui );
    lr1110_system_read_join_eui( this->radio, version_handler.join_eui );
    lr1110_system_read_pin( this->radio, version_handler.pin );

    strcpy( version_handler.version_sw, DEMO_VERSION );
    strcpy( version_handler.version_driver, lr1110_driver_version_get_version_string( ) );

    GnssHelperAlmanacDetails_t almanac_ages_crc = { 0 };
    this->GetAlmanacAgesAndCrcOfAllSatellites( &almanac_ages_crc );
    version_handler.almanac_date = almanac_ages_crc.ages_per_almanacs[0].almanac_age;
    version_handler.almanac_crc  = almanac_ages_crc.crc_almanac;
}