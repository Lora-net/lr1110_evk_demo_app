/**
 * @file      gnss_helper.cpp
 *
 * \brief
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

#include "gnss_helper.h"
#include "lr1110_gnss.h"

void GnssHelper::GetAlmanacAgesAndCrcOfAllSatellites( radio_t* radio, GnssHelperAlmanacDetails_t* almanac_details )
{
    lr1110_gnss_almanac_full_read_bytestream_t almanac_bytestream = { 0 };
    lr1110_gnss_read_almanac( radio, almanac_bytestream );
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

void GnssHelper::UpdateAlmanac( radio_t* radio, const uint8_t* almanac_buffer, const uint8_t buffer_size )
{
    if( buffer_size == LR1110_GNSS_SINGLE_ALMANAC_WRITE_SIZE )
    {
        lr1110_gnss_one_satellite_almanac_update( radio, almanac_buffer );
    }
}

bool GnssHelper::checkAlmanacUpdate( radio_t* radio, uint32_t expected_crc )
{
    bool     update_success = false;
    uint16_t result_size    = 0;
    lr1110_gnss_get_result_size( radio, &result_size );

    if( result_size == 2 )
    {
        uint8_t result_buffer[2] = { 0 };
        lr1110_gnss_read_results( radio, result_buffer, result_size );
        if( ( result_buffer[0] == 0 ) && ( result_buffer[1] == 0 ) )
        {
            uint32_t actual_crc = 0;
            lr1110_gnss_get_almanac_crc( radio, &actual_crc );
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