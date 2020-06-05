/**
 * @file      device_base.h
 *
 * @brief     Definition of device base class.
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

#ifndef __DEVICE_BASE_H__
#define __DEVICE_BASE_H__

#include "configuration.h"
#include "version.h"

#define GNSS_HELPER_NUMBER_SATELLITES_ALMANAC_READ ( 128 )

typedef struct
{
    uint8_t  sv_id;
    uint16_t almanac_age;
} GnssHelperAgeAlmanac_t;

typedef struct
{
    GnssHelperAgeAlmanac_t ages_per_almanacs[GNSS_HELPER_NUMBER_SATELLITES_ALMANAC_READ];
    uint32_t               crc_almanac;
} GnssHelperAlmanacDetails_t;

class DeviceBase
{
   public:
    explicit DeviceBase( radio_t* radio );
    virtual void ResetAndInit( )                                                                    = 0;
    virtual void FetchVersion( version_handler_t& version_handler )                                 = 0;
    virtual void GetAlmanacAgesAndCrcOfAllSatellites( GnssHelperAlmanacDetails_t* almanac_details ) = 0;
    virtual void UpdateAlmanac( const uint8_t* almanac_buffer, const uint8_t buffer_size )          = 0;
    virtual bool checkAlmanacUpdate( uint32_t expected_crc )                                        = 0;
    radio_t*     GetRadio( ) const;

   protected:
    radio_t* radio;
};

#endif  // __DEVICE_BASE_H__