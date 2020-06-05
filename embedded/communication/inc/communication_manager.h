/**
 * @file      communication_manager.h
 *
 * @brief     Definition of the communication manager.
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

#ifndef __COMMUNICATION_MANAGER_H__
#define __COMMUNICATION_MANAGER_H__

#include "environment_interface.h"
#include "communication_interface.h"
#include "hci.h"

typedef enum
{
    COMMUNICATION_MANAGER_NO_HOST,
    COMMUNICATION_MANAGER_DEMO_HOST,
    COMMUNICATION_MANAGER_FIELD_TEST_HOST,
    COMMUNICATION_MANAGER_CONNECTION_TEST_HOST,
    COMMUNICATION_MANAGER_UNKNOWN_HOST,
} CommunicationManagerHostType_t;

class CommunicationManager : public CommunicationInterface
{
   public:
    CommunicationManager( EnvironmentInterface* environment, Hci* hci );
    virtual ~CommunicationManager( );

    virtual void Runtime( ) override;
    virtual void Store( const demo_wifi_scan_all_results_t& wifi_results ) override;
    virtual void Store( const demo_gnss_all_results_t& gnss_results, uint32_t delay_since_capture ) override;
    virtual void Store( const version_handler_t& version ) override;
    virtual void EraseDataStored( ) override;
    virtual void SendDataStoredToServer( ) override;
    virtual bool GetDateAndApproximateLocation( uint32_t& gps_second, float& latitude, float& longitude,
                                                float& altitude ) override;
    virtual bool GetResults( float& latitude, float& longitude, float& altitude, float& accuracy, char* geo_coding,
                             const uint8_t geo_coding_max_length ) override;
    virtual void vLog( const char* fmt, va_list argp ) override;
    virtual bool HasNewCommand( ) const override;
    virtual CommandInterface* FetchCommand( ) override;
    virtual void              EventNotify( ) override;

    CommunicationManagerHostType_t GetHostType( ) const;
    bool                           HasHostJustChanged( CommunicationManagerHostType_t* host_type );
    bool                           SetPrintfOnlyCommunication( );
    bool                           SetDemoCommunication( );
    bool                           SetFieldTestCommunication( );

   protected:
    void SwapActiveInterface( CommunicationInterface* new_interface, CommunicationInterface** interface_to_delete );
    void HostDetectRuntime( );
    bool TestHostConnected( CommunicationManagerHostType_t* host_type );
    void SendConnectionTestResponse( );
    void SetActiveCommunicationToHostType( CommunicationManagerHostType_t host_type );
    static bool IsDemoToken( const uint8_t* buffer, const uint8_t buffer_size );
    static bool IsFieldTestToken( const uint8_t* buffer, const uint8_t buffer_size );
    static bool IsConnectionTestToken( const uint8_t* buffer, const uint8_t buffer_size );
    static bool AreBuffersEqual( const uint8_t* buffer1, const uint8_t buffer_size_1, const uint8_t* buffer2,
                                 const uint8_t buffer_size_2 );

   private:
    CommunicationInterface*        active_interface;
    CommunicationManagerHostType_t host_type;
    bool                           has_host_just_changed;
    EnvironmentInterface*          environment;
    Hci*                           hci;
    static char*                   magic_token_demo;
    static char*                   magic_token_field_test;
    static char*                   magic_token_connection_test;
};

#endif  // __COMMUNICATION_MANAGER_H__