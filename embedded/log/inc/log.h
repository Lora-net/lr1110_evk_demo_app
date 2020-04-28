/**
 * @file      log.h
 *
 * @brief     Definition of the log class.
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

#ifndef __LOGGING_H__
#define __LOGGING_H__

#include "environment_interface.h"
#include "version.h"
#include <stdint.h>

typedef enum
{
    LOGGING_NO_HOST,
    LOGGING_DEMO_HOST,
    LOGGING_FIELD_TEST_HOST,
    LOGGING_CONNECTION_TEST_HOST,
    LOGGING_UNKNOWN_HOST,
} LoggingHostType_t;

typedef enum
{
    LOGGING_STATUS_OK,
    LOGGING_STATUS_TIMEOUT,
    LOGGING_STATUS_OVERFLOW,
    LOGGING_STATUS_DISABLED,
} LoggingStatus_t;

class Logging
{
   public:
    explicit Logging( const char* module_name );
    ~Logging( );

    static void EnableLogging( );
    static void DisableLogging( );

    void        Log( const char* fmt, ... ) const;
    static void LogNoPrefix( const char* fmt, ... );
    static void Store( const char* fmt, ... );
    static void Store( const uint8_t* buffer, uint16_t size, uint32_t delay_capture, uint32_t radio_timing_ms,
                       uint32_t computation_timing_ms );
    static void SendDataStoredToServer( );
    static void SetVersion( version_handler_t* version );
    static void SendVersionInformation( );
    static void SendConnectionTestResponse( );
    static LoggingStatus_t GetResults( float& latitude, float& longitude, float& altitude, float& accuracy,
                                       char* geo_coding, const uint8_t geo_coding_max_length );
    static void            EraseDataStored( );
    static LoggingStatus_t GetDateAndApproximateLocation( uint32_t& gps_second, float& latitude, float& longitude,
                                                          float& altitude );
    static bool            TestHostConnected( LoggingHostType_t* host_type );

   protected:
    static LoggingStatus_t AskData( const char* token, const uint16_t buffer_length_max, char* buffer,
                                    uint16_t& buffer_length_received, const uint16_t timeout );
    static void            FlushSerial( );
    static void            SendCommand( const char* command );
    static LoggingStatus_t ReceiveData( const uint16_t buffer_length_max, char* buffer,
                                        uint16_t& buffer_length_received, const uint16_t timeout );

    static bool IsDemoToken( const uint8_t* buffer, const uint8_t buffer_size );
    static bool IsFieldTestToken( const uint8_t* buffer, const uint8_t buffer_size );
    static bool IsConnectionTestToken( const uint8_t* buffer, const uint8_t buffer_size );
    static bool AreBuffersEqual( const uint8_t* buffer1, const uint8_t buffer_size_1, const uint8_t* buffer2,
                                 const uint8_t buffer_size_2 );

   private:
    const char*               module_name;
    static bool               has_host_connected;
    static char*              magic_token_demo;
    static char*              magic_token_field_test;
    static char*              magic_token_connection_test;
    static bool               logging_enable;
    static version_handler_t* version_handler;
};

#endif  // __LOGGING_H__
