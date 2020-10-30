/**
 * @file      connectivity_manager_interface.h
 *
 * @brief     Definition of connectivity manager interface class.
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

#ifndef __CONNECTIVITY_MANAGER_INTERFACE_H__
#define __CONNECTIVITY_MANAGER_INTERFACE_H__

#include "device_interface.h"
#include "interruption_interface.h"
#include "lr1110_modem_lorawan.h"

#define CONNECTIVITY_MANAGER_INTERFACE_STREAM_APP_PORT ( 199 )
#define CONNECTIVITY_MANAGER_INTERFACE_MAX_DOWNLINK_BUFFER_SIZE ( 255 )
typedef enum
{
    NETWORK_CONNECTIVITY_STATUS_NO_CHANGE,
    NETWORK_CONNECTIVITY_STATUS_JOIN,
    NETWORK_CONNECTIVITY_STATUS_STREAM_DONE,
    NETWORK_CONNECTIVITY_STATUS_GOT_ALC_SYNC,
    NETWORK_CONNECTIVITY_STATUS_LOST_ALC_SYNC,
    NETWORK_CONNECTIVITY_STATUS_HAS_DOWNLINK,
} network_connectivity_status_t;

typedef struct
{
    int8_t  rssi;
    int8_t  snr;
    uint8_t flag;
    uint8_t port;
    uint8_t buffer_size;
    uint8_t buffer[CONNECTIVITY_MANAGER_INTERFACE_MAX_DOWNLINK_BUFFER_SIZE];
} network_connectivity_downlink_t;

typedef enum
{
    NETWORK_CONNECTIVITY_REGION_EU868,
    NETWORK_CONNECTIVITY_REGION_US915,
} network_connectivity_region_t;

typedef enum
{
    NETWORK_CONNECTIVITY_CMD_STATUS_OK,
    NETWORK_CONNECTIVITY_CMD_STATUS_BUFFER_TOO_SHORT,
    NETWORK_CONNECTIVITY_CMD_STATUS_ERROR,
} network_connectivity_cmd_status_t;

typedef enum
{
    NETWORK_CONNECTIVITY_ADR_NETWORK_SERVER_CONTROLLED,
    NETWORK_CONNECTIVITY_ADR_MOBILE_LONG_RANGE,
    NETWORK_CONNECTIVITY_ADR_MOBILE_LOW_POWER,
} network_connectivity_adr_profile_t;

typedef struct
{
    network_connectivity_region_t      region;
    network_connectivity_adr_profile_t adr_profile;
} network_connectivity_settings_t;

class ConnectivityManagerInterface
{
   public:
    explicit ConnectivityManagerInterface( );
    virtual ~ConnectivityManagerInterface( );

    virtual network_connectivity_status_t Runtime( ) = 0;

    virtual bool                              IsConnectable( ) const                                      = 0;
    virtual void                              Join( network_connectivity_settings_t* settings )           = 0;
    virtual void                              Leave( )                                                    = 0;
    virtual network_connectivity_cmd_status_t Send( uint8_t* data, uint8_t length )                       = 0;
    virtual void                              GetCurrentRegion( network_connectivity_region_t* region )   = 0;
    virtual void                              SetRegion( network_connectivity_region_t region )           = 0;
    virtual void                              SetAdrProfile( network_connectivity_adr_profile_t profile ) = 0;
    virtual void                              ResetCommissioningToSemtechJoinServer( )                    = 0;

    virtual void InterruptHandler( const InterruptionInterface* interruption ) = 0;

    bool getTimeSyncState( );

    /**
     * \brief Check if a new downlink is available and if so, copy it in the provided pointer
     *
     * Implements a flush-like behavior so that if a call returns true, a successive call will return false (unless a
     * new downlink was received)
     *
     * \param[in] new_downlink Pointer to a memory allocated where to copy the new downlink if available. If there is no
     *new downlink available, the memory pointed to is not modified
     *
     * \retval true A new downlink is available and has been copied in the memory pointed to
     *
     * \retval false No downlink were avaialble and the memory pointed to by new_downlink has not been modified
     */
    bool FetchNewDownlink( network_connectivity_downlink_t* new_downlink );

    /**
     * \brief Cheeck wether a new downlink is available
     *
     * \retval true A new downlink is available
     *
     * \retval false No new downlink is available
     *
     * \see FetchNewDownlink
     */
    bool HasNewDownlink( ) const;

   protected:
    bool                            _is_time_sync;
    bool                            _is_joined;
    network_connectivity_settings_t _settings;
    static const uint8_t            SEMTECH_DEFAULT_JOIN_EUI[LR1110_MODEM_DM_MESSAGE_JOIN_EUI_LENGTH];
    network_connectivity_downlink_t _last_downlink;
    bool                            _has_new_downlink;
};

#endif  // __CONNECTIVITY_MANAGER_INTERFACE_H__
