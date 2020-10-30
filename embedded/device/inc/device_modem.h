/**
 * @file      device_modem.h
 *
 * @brief     Definition of modem device class.
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

#ifndef __DEVICE_MODEM_H__
#define __DEVICE_MODEM_H__

#include "device_interface.h"
#include "interruption_modem.h"
#include "lr1110_modem_common.h"
#include "environment_interface.h"
#include "lr1110_modem_gnss.h"

class DeviceModem : public DeviceInterface
{
   public:
    explicit DeviceModem( radio_t* radio, EnvironmentInterface* environment );
    void FetchVersion( version_handler_t& version_handler ) override;
    void Init( ) override;
    void GetAlmanacAgesAndCrcOfAllSatellites( GnssHelperAlmanacDetails_t* almanac_details ) override;
    void GetAlmanacAgesForSatelliteId( uint8_t sv_id, uint16_t* almanac_age ) override;
    void UpdateAlmanac( const uint8_t* almanac_buffer, const uint8_t buffer_size ) override;
    bool FetchInterrupt( InterruptionInterface** interruption ) override;
    bool IsLorawanPortForDeviceManagement( const uint8_t port ) const override;
    void HandleLorawanDeviceManagement( const uint8_t port, const uint8_t* payload,
                                        const uint8_t payload_length ) override;
    bool checkAlmanacUpdate( uint32_t expected_crc ) override;
    void NotifyEnvironmentChange( ) override;
    void FetchAssistanceLocation( DeviceAssistedLocation_t* assistance_location ) override;

   protected:
    void        SetAssistancePositionFromEnvironment( );
    static void GnssPositionFromEnvironment( const environment_location_t&                   location,
                                             lr1110_modem_gnss_solver_assistance_position_t& gnss_position );
    bool        HasAssistedLocationUpdated( ) override;

    InterruptionModem interruption;

   private:
    bool has_assisted_location_update;
};

#endif  // __DEVICE_MODEM_H__
