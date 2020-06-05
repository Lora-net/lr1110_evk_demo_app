/**
 * @file      command_get_almanac_dates.cpp
 *
 * @brief     Implementation of the HCI command to get the almanac dates class.
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

#include "command_get_almanac_dates.h"
#include "com_code.h"
#include "demo.h"
#include <stdio.h>

#define N_SATELLITES ( 128 )
#define ALMANAC_DATES_STRING_MAX_LEN ( 3 * N_SATELLITES )

CommandGetAlmanacDates::CommandGetAlmanacDates( DeviceBase* device, Hci& hci ) : hci( &hci ), device( device ) {}

CommandGetAlmanacDates::~CommandGetAlmanacDates( ) {}

bool CommandGetAlmanacDates::ConfigureFromPayload( const uint8_t* buffer, const uint16_t buffer_length )
{
    bool success = false;
    if( buffer_length == 0 )
    {
        success = true;
    }
    else
    {
        success = false;
    }
    return success;
}

CommandEvent_t CommandGetAlmanacDates::Execute( )
{
    char           buffer_response[ALMANAC_DATES_STRING_MAX_LEN] = { 0 };
    const uint16_t buffer_len                                    = ALMANAC_DATES_STRING_MAX_LEN;

    GnssHelperAlmanacDetails_t almanac_ages_and_crc = { 0 };
    this->device->GetAlmanacAgesAndCrcOfAllSatellites( &almanac_ages_and_crc );

    for( uint8_t index_sat = 0; index_sat < N_SATELLITES; index_sat++ )
    {
        const uint16_t                   local_buffer_index = index_sat * 3;
        const uint16_t                   local_date = almanac_ages_and_crc.ages_per_almanacs[index_sat].almanac_age;
        const lr1110_gnss_satellite_id_t local_satellite = almanac_ages_and_crc.ages_per_almanacs[index_sat].sv_id;

        buffer_response[local_buffer_index + 0] = local_satellite;
        buffer_response[local_buffer_index + 1] = ( uint8_t )( ( local_date >> 8 ) & 0x00FF );
        buffer_response[local_buffer_index + 2] = ( uint8_t )( local_date & 0x00FF );
    }

    this->hci->SendResponse( this->GetComCode( ), ( uint8_t* ) buffer_response, buffer_len );
    return COMMAND_NO_EVENT;
}

uint16_t CommandGetAlmanacDates::GetComCode( ) { return COM_CODE_GET_ALMANAC_DATES; }
