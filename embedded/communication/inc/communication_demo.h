/**
 * @file      communication_demo.h
 *
 * @brief     Definition of the communication demo.
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

#ifndef __COMMUNICATION_DEMO_H__
#define __COMMUNICATION_DEMO_H__

#include "communication_interface.h"

typedef enum
{
    COMMUNICATION_DEMO_STATUS_OK,
    COMMUNICATION_DEMO_STATUS_TIMEOUT,
    COMMUNICATION_DEMO_STATUS_OVERFLOW,
    COMMUNICATION_DEMO_STATUS_DISABLED,
} CommunicationDemoStatus_t;

class CommunicationDemo : public CommunicationInterface
{
   public:
    virtual void Runtime( ) override;
    virtual bool GetDateAndApproximateLocation( uint32_t& gps_second, float& latitude, float& longitude,
                                                float& altitude ) override;
    virtual bool GetResults( float& latitude, float& longitude, float& altitude, float& accuracy, char* geo_coding,
                             const uint8_t geo_coding_max_length );
    virtual void Store( const demo_wifi_scan_all_results_t& wifi_results );
    virtual void Store( const demo_gnss_all_results_t& gnss_results, uint32_t delay_since_capture );
    virtual void Store( const version_handler_t& version );
    virtual void EraseDataStored( ) override;
    virtual void SendDataStoredToServer( ) override;
    virtual void vLog( const char* fmt, va_list argp ) override;
    virtual bool HasNewCommand( ) const override;
    virtual CommandInterface* FetchCommand( ) override;

   protected:
    CommunicationDemoStatus_t AskAndParseResults( float& latitude, float& longitude, float& altitude, float& accuracy,
                                                  char* geo_coding, const uint8_t geo_coding_max_length );
    CommunicationDemoStatus_t ReceiveData( const uint16_t buffer_length_max, char* buffer,
                                           uint16_t& buffer_length_received, const uint16_t timeout );
    CommunicationDemoStatus_t AskData( const char* token, const uint16_t buffer_length_max, char* buffer,
                                       uint16_t& buffer_length_received, const uint16_t timeout );
    void                      SendCommand( const char* command );
    void                      Store( const char* fmt, ... );
};

#endif  // __COMMUNICATION_DEMO_H__