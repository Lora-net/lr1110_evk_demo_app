/**
 * @file      environment_interface.h
 *
 * @brief     Interface definition for interactions with the environment.
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

#ifndef __ENVIRONMENT_INTERFACE_H__
#define __ENVIRONMENT_INTERFACE_H__

#include <time.h>
#include <stdint.h>

typedef struct
{
    float latitude;
    float longitude;
    float altitude;
} environment_location_t;

typedef time_t environment_date_time_t;

/*
 * Is responsible to handle the approximate location and time of the device
 * Mainly used for Gnss Assisted Scan
 */
class EnvironmentInterface
{
   public:
    EnvironmentInterface( );
    virtual ~EnvironmentInterface( );

    bool HasDate( ) const;
    bool HasLocation( ) const;

    environment_date_time_t GetDateTime( ) const;
    environment_location_t  GetLocation( ) const;

    void           SetTimeFromGpsEpoch( const environment_date_time_t& time_from_gps_epoch );
    void           SetLocation( const environment_location_t& location );
    int32_t        GetDelta( ) const { return this->delta_time_s; }
    virtual time_t GetLocalTimeSeconds( ) const      = 0;
    virtual time_t GetLocalTimeMilliseconds( ) const = 0;

   private:
    bool                   has_date;
    bool                   has_location;
    int32_t                delta_time_s;
    environment_location_t last_location;
};

#endif  // __ENVIRONMENT_INTERFACE_H__
