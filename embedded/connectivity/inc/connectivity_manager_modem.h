/**
 * @file      connectivity_manager_modem.h
 *
 * @brief     Definition of modem connectivity class.
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

#ifndef __CONNECTIVITY_MANAGER_MODEM_H__
#define __CONNECTIVITY_MANAGER_MODEM_H__

#include "connectivity_manager_interface.h"
#include "device_modem.h"
#include "lr1110_modem_lorawan.h"

class ConnectivityManagerModem : public ConnectivityManagerInterface
{
   public:
    explicit ConnectivityManagerModem( DeviceModem* device );
    virtual ~ConnectivityManagerModem( );

    network_connectivity_status_t Runtime( ) final;

    bool                              IsConnectable( ) const final;
    void                              Join( network_connectivity_settings_t* settings ) final;
    void                              Leave( ) final;
    network_connectivity_cmd_status_t Send( uint8_t* data, uint8_t length ) final;
    void                              GetCurrentRegion( network_connectivity_region_t* region ) final;
    void                              SetRegion( network_connectivity_region_t region ) final;
    void                              SetAdrProfile( network_connectivity_adr_profile_t profile ) final;
    void                              ResetCommissioningToSemtechJoinServer( ) final;

    void InterruptHandler( const InterruptionInterface* interruption ) final;

   private:
    DeviceModem*                _device;
    bool                        _has_received_event;
    lr1110_modem_event_fields_t _last_received_event;
};

#endif  // __CONNECTIVITY_MANAGER_MODEM_H__
