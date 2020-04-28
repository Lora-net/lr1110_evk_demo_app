/**
 * @file      gnss_helper.h
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

#ifndef __GNSS_HELPER_H__
#define __GNSS_HELPER_H__

#include <stdint.h>
#include "lr1110_gnss_types.h"
#include "configuration.h"

#define GNSS_HELPER_NUMBER_SATELLITES_ALMANAC_READ ( LR1110_GNSS_FULL_UPDATE_N_ALMANACS )

typedef struct
{
    lr1110_gnss_satellite_id_t sv_id;
    uint16_t                   almanac_age;
} GnssHelperAgeAlmanac_t;

typedef struct
{
    GnssHelperAgeAlmanac_t ages_per_almanacs[GNSS_HELPER_NUMBER_SATELLITES_ALMANAC_READ];
    uint32_t               crc_almanac;
} GnssHelperAlmanacDetails_t;

class GnssHelper
{
   public:
    virtual ~GnssHelper( );

    /*!
     * \brief Get the age and CRC 16bits of the almanac for the satellites given
     * in arguments
     *
     * The age of the almanac is expressed in days since last GPS week rollback
     *
     * \param [in] sv_ids The list of satellite IDs to get almanac. Length must
     * be at least n_sv
     *
     * \param [in] n_sv The number of satellites to get almanac from
     *
     * \param [out] age_per_satellites Array of uint16_t to be filled with age
     * and CRC of the almanac for the satellites provided in input. The order in
     * the array is the number of satellite IDs in sv_ids. It is up to the
     * client of this method to provide a storage of at least n_sv structure for
     * this array
     */
    static void GetAlmanacAgesAndCrcOfAllSatellites( radio_t* radio, GnssHelperAlmanacDetails_t* almanac_details );

    static void UpdateAlmanac( radio_t* radio, const uint8_t* almanac_buffer, const uint8_t buffer_size );

    static bool checkAlmanacUpdate( radio_t* radio, uint32_t expected_crc );

   protected:
    GnssHelper( ){};
};

#endif  // __GNSS_HELPER_H__
