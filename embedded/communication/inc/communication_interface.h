/**
 * @file      communication_interface.h
 *
 * @brief     Definition of the communication interface.
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

#ifndef __COMMUNICATION_INTERFACE_H__
#define __COMMUNICATION_INTERFACE_H__

#include <stdarg.h>
#include "command_interface.h"
#include "version.h"
#include "demo_wifi_types.h"
#include "demo_gnss_types.h"

class CommunicationInterface
{
   public:
    virtual ~CommunicationInterface( );
    virtual void Init( ){};
    virtual void DeInit( ){};
    virtual void Runtime( ) = 0;
    virtual void Log( const char* fmt, ... );
    virtual void vLog( const char* fmt, va_list argp )                                              = 0;
    virtual void Store( const demo_wifi_scan_all_results_t& wifi_results )                          = 0;
    virtual void Store( const demo_gnss_all_results_t& gnss_results, uint32_t delay_since_capture ) = 0;
    virtual void Store( const version_handler_t& version )                                          = 0;
    virtual void EraseDataStored( )                                                                 = 0;
    virtual void SendDataStoredToServer( )                                                          = 0;
    virtual bool GetDateAndApproximateLocation( uint32_t& gps_second, float& latitude, float& longitude,
                                                float& altitude )                                   = 0;
    virtual bool GetResults( float& latitude, float& longitude, float& altitude, float& accuracy, char* geo_coding,
                             const uint8_t geo_coding_max_length )                                  = 0;
    virtual void EventNotify( );
    virtual bool HasNewCommand( ) const       = 0;
    virtual CommandInterface* FetchCommand( ) = 0;

   protected:
    static const char* WifiTypeToStr( const demo_wifi_signal_type_t type );
};

#endif  // __COMMUNICATION_INTERFACE_H__