/**
 * @file      communication_manager.cpp
 *
 * @brief     Implementation of the communication manager responsible for
 * communication interface switch.
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

#include <stdio.h>
#include <string.h>
#include "communication_utils.h"
#include "communication_manager.h"
#include "communication_print_only.h"
#include "communication_demo.h"
#include "communication_field_test.h"

#define COMMUNICATION_MANAGER_MAGIC_TOKEN_SIZE ( 10 )
#define COMMUNICATION_MANAGER_TIMEOUT_SERIAL_SHORT_RECEIVE_MS ( 100 )
#define COMMUNICATION_MANAGER_MAGIC_TOKEN_DEMO "demooglog"
#define COMMUNICATION_MANAGER_MAGIC_TOKEN_FIELD_TEST "fieldglog"
#define COMMUNICATION_MANAGER_MAGIC_TOKEN_CONNECTION_TEST "testdglog"

char* CommunicationManager::magic_token_demo            = ( char* ) COMMUNICATION_MANAGER_MAGIC_TOKEN_DEMO;
char* CommunicationManager::magic_token_field_test      = ( char* ) COMMUNICATION_MANAGER_MAGIC_TOKEN_FIELD_TEST;
char* CommunicationManager::magic_token_connection_test = ( char* ) COMMUNICATION_MANAGER_MAGIC_TOKEN_CONNECTION_TEST;

CommunicationManager::CommunicationManager( EnvironmentInterface* environment, Hci* hci )
    : active_interface( new CommunicationPrintOnly( ) ),
      host_type( COMMUNICATION_MANAGER_NO_HOST ),
      has_host_just_changed( false ),
      environment( environment ),
      hci( hci )
{
}

CommunicationManager::~CommunicationManager( ) { delete this->active_interface; }

bool CommunicationManager::TestHostConnected( CommunicationManagerHostType_t* host_type )
{
    char     reception_buffer[COMMUNICATION_MANAGER_MAGIC_TOKEN_SIZE] = { 0x00 };
    uint16_t reception_buffer_size                                    = 0;
    printf( "!TEST_HOST\n" );
    CommunicationUtilsReceptionStatus_t receive_status =
        CommunicationUtilsReceiveData( COMMUNICATION_MANAGER_MAGIC_TOKEN_SIZE, reception_buffer, reception_buffer_size,
                                       COMMUNICATION_MANAGER_TIMEOUT_SERIAL_SHORT_RECEIVE_MS );
    if( receive_status != COMMUNICATION_UTILS_RECEPTION_UTILS_STATUS_OK )
    {
        *host_type = COMMUNICATION_MANAGER_NO_HOST;
        return false;
    }
    else
    {
        if( this->IsDemoToken( ( uint8_t* ) reception_buffer, reception_buffer_size ) )
        {
            *host_type = COMMUNICATION_MANAGER_DEMO_HOST;
        }
        else if( this->IsFieldTestToken( ( uint8_t* ) reception_buffer, reception_buffer_size ) )
        {
            *host_type = COMMUNICATION_MANAGER_FIELD_TEST_HOST;
        }
        else if( this->IsConnectionTestToken( ( uint8_t* ) reception_buffer, reception_buffer_size ) )
        {
            *host_type = COMMUNICATION_MANAGER_CONNECTION_TEST_HOST;
        }
        else
        {
            *host_type = COMMUNICATION_MANAGER_UNKNOWN_HOST;
        }
        return true;
    }
}

void CommunicationManager::Runtime( )
{
    this->HostDetectRuntime( );
    this->active_interface->Runtime( );
}

void CommunicationManager::Store( const demo_wifi_scan_all_results_t& wifi_results )
{
    this->active_interface->Store( wifi_results );
}

void CommunicationManager::Store( const demo_gnss_all_results_t& gnss_results, const uint32_t delay_since_capture )
{
    this->active_interface->Store( gnss_results, delay_since_capture );
}

void CommunicationManager::Store( const version_handler_t& version ) { this->active_interface->Store( version ); }

void CommunicationManager::EraseDataStored( ) { this->active_interface->EraseDataStored( ); }

void CommunicationManager::SendDataStoredToServer( ) { this->active_interface->SendDataStoredToServer( ); }

bool CommunicationManager::GetDateAndApproximateLocation( uint32_t& gps_second, float& latitude, float& longitude,
                                                          float& altitude )
{
    const bool success =
        this->active_interface->GetDateAndApproximateLocation( gps_second, latitude, longitude, altitude );
    if( ( this->host_type == COMMUNICATION_MANAGER_DEMO_HOST ) && ( success == false ) )
    {
        // If host type is demo, and the get date calls failed, it means something wrong happened on the line. Swith
        // the current interface to NO HOST
        this->SetActiveCommunicationToHostType( COMMUNICATION_MANAGER_NO_HOST );
    }
    return success;
}

bool CommunicationManager::GetResults( float& latitude, float& longitude, float& altitude, float& accuracy,
                                       char* geo_coding, const uint8_t geo_coding_max_length )
{
    const bool success = this->active_interface->GetResults( latitude, longitude, altitude, accuracy, geo_coding,
                                                             geo_coding_max_length );
    if( ( this->host_type == COMMUNICATION_MANAGER_DEMO_HOST ) && ( success == false ) )
    {
        // If host type is demo, and the get result calls failed, it means something wrong happened on the line. Swith
        // the current interface to NO HOST
        this->SetActiveCommunicationToHostType( COMMUNICATION_MANAGER_NO_HOST );
    }
    return success;
}

void CommunicationManager::vLog( const char* fmt, va_list argp ) { this->active_interface->vLog( fmt, argp ); }

bool CommunicationManager::HasNewCommand( ) const { return this->active_interface->HasNewCommand( ); }

CommandInterface* CommunicationManager::FetchCommand( ) { return this->active_interface->FetchCommand( ); }

void CommunicationManager::EventNotify( ) { this->active_interface->EventNotify( ); }

CommunicationManagerHostType_t CommunicationManager::GetHostType( ) const { return this->host_type; }

bool CommunicationManager::HasHostJustChanged( CommunicationManagerHostType_t* host_type )
{
    const bool has_changed = this->has_host_just_changed;
    if( this->has_host_just_changed == true )
    {
        *host_type                  = this->host_type;
        this->has_host_just_changed = false;
    }
    return has_changed;
}

bool CommunicationManager::SetPrintfOnlyCommunication( )
{
    CommunicationInterface* tmp_interface_pointer = new CommunicationPrintOnly( );
    bool                    success               = false;
    if( tmp_interface_pointer != nullptr )
    {
        // Only delete previous active_interface if creating the new one succeeded.
        // Otherwise, the instance will stay with a dangling pointer and destructor call will break something
        CommunicationInterface* interface_to_delete = nullptr;
        this->SwapActiveInterface( tmp_interface_pointer, &interface_to_delete );
        delete interface_to_delete;
        success = true;
    }
    else
    {
        success = false;
    }
    return success;
}

bool CommunicationManager::SetDemoCommunication( )
{
    CommunicationInterface* tmp_interface_pointer = new CommunicationDemo( );
    bool                    success               = false;
    if( tmp_interface_pointer != nullptr )
    {
        // Only delete previous active_interface if creating the new one succeeded.
        // Otherwise, the instance will stay with a dangling pointer and destructor call will break something
        CommunicationInterface* interface_to_delete = nullptr;
        this->SwapActiveInterface( tmp_interface_pointer, &interface_to_delete );
        delete interface_to_delete;
        success = true;
    }
    else
    {
        success = false;
    }
    return success;
}

bool CommunicationManager::SetFieldTestCommunication( )
{
    CommunicationInterface* tmp_interface_pointer = new CommunicationFieldTest( this->hci );
    bool                    success               = false;
    if( tmp_interface_pointer != nullptr )
    {
        // Only delete previous active_interface if creating the new one succeeded.
        // Otherwise, the instance will stay with a dangling pointer and destructor call will break something
        CommunicationInterface* interface_to_delete = nullptr;
        this->SwapActiveInterface( tmp_interface_pointer, &interface_to_delete );
        delete interface_to_delete;
        success = true;
    }
    else
    {
        success = false;
    }
    return success;
}

void CommunicationManager::SwapActiveInterface( CommunicationInterface*  new_interface,
                                                CommunicationInterface** interface_to_delete )
{
    this->active_interface->DeInit( );
    *interface_to_delete   = this->active_interface;
    this->active_interface = new_interface;
    this->active_interface->Init( );
}

void CommunicationManager::HostDetectRuntime( )
{
    static time_t                        last_fetch_s                   = 0;
    const uint16_t                       host_fetch_if_unknown_period_s = 1;
    const time_t                         actual_time                    = this->environment->GetLocalTimeSeconds( );
    const CommunicationManagerHostType_t last_type                      = this->host_type;
    CommunicationManagerHostType_t       new_type                       = this->host_type;
    switch( this->host_type )
    {
    case COMMUNICATION_MANAGER_NO_HOST:
    case COMMUNICATION_MANAGER_UNKNOWN_HOST:
    case COMMUNICATION_MANAGER_CONNECTION_TEST_HOST:
    case COMMUNICATION_MANAGER_DEMO_HOST:
    {
        if( ( actual_time - last_fetch_s ) > host_fetch_if_unknown_period_s )
        {
            last_fetch_s = actual_time;
            this->TestHostConnected( &new_type );
            if( new_type == COMMUNICATION_MANAGER_CONNECTION_TEST_HOST )
            {
                this->SendConnectionTestResponse( );
            }
        }
        break;
    }
    default:
    {
        break;
    }
    }

    if( new_type != last_type )
    {
        this->SetActiveCommunicationToHostType( new_type );
    }
}

void CommunicationManager::SendConnectionTestResponse( ) { this->Log( "It works !\n" ); }

bool CommunicationManager::IsDemoToken( const uint8_t* buffer, const uint8_t buffer_size )
{
    return CommunicationManager::AreBuffersEqual( buffer, buffer_size,
                                                  ( uint8_t* ) CommunicationManager::magic_token_demo,
                                                  COMMUNICATION_MANAGER_MAGIC_TOKEN_SIZE );
}

bool CommunicationManager::IsFieldTestToken( const uint8_t* buffer, const uint8_t buffer_size )
{
    return CommunicationManager::AreBuffersEqual( buffer, buffer_size,
                                                  ( uint8_t* ) CommunicationManager::magic_token_field_test,
                                                  COMMUNICATION_MANAGER_MAGIC_TOKEN_SIZE );
}

bool CommunicationManager::IsConnectionTestToken( const uint8_t* buffer, const uint8_t buffer_size )
{
    return CommunicationManager::AreBuffersEqual( buffer, buffer_size,
                                                  ( uint8_t* ) CommunicationManager::magic_token_connection_test,
                                                  COMMUNICATION_MANAGER_MAGIC_TOKEN_SIZE );
}

bool CommunicationManager::AreBuffersEqual( const uint8_t* buffer1, const uint8_t buffer_size_1, const uint8_t* buffer2,
                                            const uint8_t buffer_size_2 )
{
    if( buffer_size_1 != buffer_size_2 )
    {
        return false;
    }
    return ( memcmp( buffer1, buffer2, buffer_size_1 ) == 0 );
}

void CommunicationManager::SetActiveCommunicationToHostType( CommunicationManagerHostType_t host_type )
{
    if( this->host_type == host_type )
    {
        // Do nothing if we were about to set the same interface as the one already active
        return;
    }

    bool success = false;
    switch( host_type )
    {
    case COMMUNICATION_MANAGER_NO_HOST:
    {
        success = this->SetPrintfOnlyCommunication( );
        break;
    }

    case COMMUNICATION_MANAGER_DEMO_HOST:
    {
        success = this->SetDemoCommunication( );
        break;
    }

    case COMMUNICATION_MANAGER_FIELD_TEST_HOST:
    {
        success = this->SetFieldTestCommunication( );
        break;
    }

    case COMMUNICATION_MANAGER_CONNECTION_TEST_HOST:
    {
        success = this->SetFieldTestCommunication( );
        break;
    }

    case COMMUNICATION_MANAGER_UNKNOWN_HOST:
    {
        success = this->SetPrintfOnlyCommunication( );
        break;
    }
    }
    if( success == true )
    {
        // Change the host type only if the switch was successfull
        this->has_host_just_changed = true;
        this->host_type             = host_type;
    }
}