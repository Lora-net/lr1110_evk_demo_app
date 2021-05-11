/**
 * @file      connectivity_manager_interface.cpp
 *
 * @brief     Implementation of connectivity manager interface class
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

#include <string.h>  // memcpy
#include "connectivity_manager_interface.h"

const uint8_t ConnectivityManagerInterface::SEMTECH_DEFAULT_JOIN_EUI[LR1110_MODEM_JOIN_EUI_LENGTH] = {
    0x00, 0x16, 0xC0, 0x01, 0xFF, 0xFE, 0x00, 0x01,
};

ConnectivityManagerInterface::ConnectivityManagerInterface( )
    : _is_time_sync( false ), _is_joined( false ), _has_new_downlink( false )
{
}

ConnectivityManagerInterface::~ConnectivityManagerInterface( ) {}

bool ConnectivityManagerInterface::getTimeSyncState( ) { return this->_is_time_sync; }

bool ConnectivityManagerInterface::FetchNewDownlink( network_connectivity_downlink_t* new_downlink )
{
    bool success = false;
    if( this->_has_new_downlink == true )
    {
        success           = true;
        ( *new_downlink ) = this->_last_downlink;
        memcpy( new_downlink->buffer, this->_last_downlink.buffer, this->_last_downlink.buffer_size );
        this->_has_new_downlink = false;
    }
    else
    {
        success = false;
    }
    return success;
}

bool ConnectivityManagerInterface::HasNewDownlink( ) const { return this->_has_new_downlink; }