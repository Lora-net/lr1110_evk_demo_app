/**
 * @file      connectivity_manager_transceiver.cpp
 *
 * @brief     Implementation of connectivity manager for transceiver.
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

#include "connectivity_manager_transceiver.h"

ConnectivityManagerTransceiver::ConnectivityManagerTransceiver( ) {}

ConnectivityManagerTransceiver::~ConnectivityManagerTransceiver( ) {}

network_connectivity_status_t ConnectivityManagerTransceiver::Runtime( )
{
    // Has of today there is no connectivity with the transceiver
    return NETWORK_CONNECTIVITY_STATUS_NO_CHANGE;
}

bool ConnectivityManagerTransceiver::IsConnectable( ) const { return false; }

void ConnectivityManagerTransceiver::Join( network_connectivity_settings_t* settings )
{
    ( void ) settings;
    return;
}

void ConnectivityManagerTransceiver::Leave( ) { return; }

network_connectivity_cmd_status_t ConnectivityManagerTransceiver::Send( uint8_t* data, uint8_t length )
{
    ( void ) data;
    ( void ) length;
    return NETWORK_CONNECTIVITY_CMD_STATUS_ERROR;
}

void ConnectivityManagerTransceiver::GetCurrentRegion( network_connectivity_region_t* region )
{
    ( void ) region;
    return;
}

void ConnectivityManagerTransceiver::SetRegion( network_connectivity_region_t region )
{
    ( void ) region;
    return;
}

void ConnectivityManagerTransceiver::SetAdrProfile( network_connectivity_adr_profile_t profile )
{
    ( void ) profile;
    return;
}

void ConnectivityManagerTransceiver::ResetCommissioningToSemtechJoinServer( ) {}

void ConnectivityManagerTransceiver::InterruptHandler( const InterruptionInterface* interruption )
{
    ( void ) interruption;
    return;
}