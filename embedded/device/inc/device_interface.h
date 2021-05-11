/**
 * @file      device_interface.h
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

#ifndef __DEVICE_INTERFACE_H__
#define __DEVICE_INTERFACE_H__

#include "configuration.h"
#include "version.h"
#include "interruption_interface.h"
#include "environment_interface.h"
#include "application_server_interpreter.h"

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

typedef enum
{
    DEVICE_EVENT_NONE,
    DEVICE_EVENT_ASSISTANCE_LOCATION_UPDATED,
    DEVICE_EVENT_APPLICATION_SERVER,
} DeviceEvent_t;

typedef struct
{
    float latitude;
    float longitude;
} DeviceAssistedLocation_t;

class DeviceInterface
{
   public:
    DeviceInterface( radio_t* radio, EnvironmentInterface* environment );
    DeviceEvent_t Runtime( );

    virtual void     Init( )                                                                            = 0;
    virtual void     FetchVersion( version_handler_t& version_handler )                                 = 0;
    virtual void     GetAlmanacAgesAndCrcOfAllSatellites( GnssHelperAlmanacDetails_t* almanac_details ) = 0;
    virtual void     GetAlmanacAgesForSatelliteId( uint8_t sv_id, uint16_t* almanac_age )               = 0;
    virtual void     UpdateAlmanac( const uint8_t* almanac_buffer, const uint8_t buffer_size )          = 0;
    virtual bool     checkAlmanacUpdate( uint32_t expected_crc )                                        = 0;
    virtual bool     FetchInterrupt( InterruptionInterface** interruption )                             = 0;
    virtual bool     IsLorawanPortForDeviceManagement( const uint8_t port ) const                       = 0;
    virtual void     HandleLorawanDeviceManagement( const uint8_t port, const uint8_t* payload,
                                                    const uint8_t payload_length )                      = 0;
    virtual void     NotifyEnvironmentChange( )                                                         = 0;
    virtual radio_t* GetRadio( ) const;
    virtual void     FetchAssistanceLocation( DeviceAssistedLocation_t* assistance_location )                   = 0;
    virtual void     FetchLastApplicationServerEvent( ApplicationServerEvent_t* last_application_server_event ) = 0;

   protected:
    virtual bool HasAssistedLocationUpdated( ) = 0;
    virtual bool HasApplicationServerEvent( )  = 0;

    radio_t*              radio;
    EnvironmentInterface* environment;
};

#endif  // __DEVICE_INTERFACE_H__
