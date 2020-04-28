/**
 * @file      log.cpp
 *
 * @brief     Implementation of the log class.
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

#include "log.h"
#include "system_time.h"
#include "system_uart.h"
#include <stdarg.h>
#include <stdio.h>

#define LOG_TIMEOUT_SERIAL_RECEIVE_MS ( 2500 )
#define LOG_TIMEOUT_SERIAL_SHORT_RECEIVE_MS ( 100 )
#define LOG_MAGIC_TOKEN_SIZE ( 10 )
#define LOG_MAGIC_TOKEN_DEMO "demooglog"
#define LOG_MAGIC_TOKEN_FIELD_TEST "fieldglog"
#define LOG_MAGIC_TOKEN_CONNECTION_TEST "testdglog"
#define LOG_GET_RESULT_BUFFER_LENGTH 128
#define LOG_TMP_FORMAT_LENGTH ( 32 )

#define LOG_COMMAND_TOKEN_SEND "SEND"
#define LOG_COMMAND_TOKEN_FLUSH "FLUSH"
#define LOG_COMMAND_TOKEN_DATE "DATE"
#define LOG_COMMAND_TOKEN_RESULT "RESULT"
#define LOG_COMMAND_TOKEN_TEST_HOST "TEST_HOST"
#define LOG_COMMAND_TOKEN_STORE_VERSION "VERSION"

bool               Logging::has_host_connected          = false;
char*              Logging::magic_token_demo            = ( char* ) LOG_MAGIC_TOKEN_DEMO;
char*              Logging::magic_token_field_test      = ( char* ) LOG_MAGIC_TOKEN_FIELD_TEST;
char*              Logging::magic_token_connection_test = ( char* ) LOG_MAGIC_TOKEN_CONNECTION_TEST;
bool               Logging::logging_enable              = false;
version_handler_t* Logging::version_handler;

Logging::Logging( const char* module_name ) : module_name( module_name ) {}

Logging::~Logging( ) {}

void Logging::EnableLogging( ) { Logging::logging_enable = true; }

void Logging::DisableLogging( ) { Logging::logging_enable = false; }

void Logging::Log( const char* fmt, ... ) const
{
    if( Logging::logging_enable )
    {
        printf( "# [%s] ", this->module_name );
        va_list args;
        va_start( args, fmt );
        vprintf( fmt, args );
        va_end( args );
    }
}

void Logging::LogNoPrefix( const char* fmt, ... )
{
    if( Logging::logging_enable )
    {
        va_list args;
        va_start( args, fmt );
        vprintf( fmt, args );
        va_end( args );
    }
}

void Logging::Store( const char* fmt, ... )
{
    if( Logging::logging_enable )
    {
        printf( "@" );
        va_list args;
        va_start( args, fmt );
        vprintf( fmt, args );
        va_end( args );
    }
}

void Logging::Store( const uint8_t* buffer, uint16_t size, uint32_t delay_capture, uint32_t radio_timing_ms,
                     uint32_t computation_timing_ms )
{
    printf( "@" );
    for( uint16_t index = 0; index < size; index++ )
    {
        printf( "%02x", buffer[index] );
    }
    printf( ", %u, %u, %u", delay_capture, radio_timing_ms, computation_timing_ms );
    printf( "\n" );
}

void Logging::SetVersion( version_handler_t* version ) { Logging::version_handler = version; }

void Logging::SendVersionInformation( )
{
    Logging::Store( "%s:%s:%s:0x%04x:0x%x:%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n",
                    LOG_COMMAND_TOKEN_STORE_VERSION,
                    Logging::version_handler->version_sw,
                    Logging::version_handler->version_driver,
                    Logging::version_handler->version_chip_fw,
                    Logging::version_handler->almanac_date,
                    Logging::version_handler->chip_uid[0],
                    Logging::version_handler->chip_uid[1],
                    Logging::version_handler->chip_uid[2],
                    Logging::version_handler->chip_uid[3],
                    Logging::version_handler->chip_uid[4],
                    Logging::version_handler->chip_uid[5],
                    Logging::version_handler->chip_uid[6],
                    Logging::version_handler->chip_uid[7] );
}

void Logging::SendConnectionTestResponse( ) { Logging::LogNoPrefix( "It works !\n" ); }

bool Logging::TestHostConnected( LoggingHostType_t* host_type )
{
    char            reception_buffer[LOG_MAGIC_TOKEN_SIZE] = { 0x00 };
    uint16_t        reception_buffer_size                  = 0;
    LoggingStatus_t status = Logging::AskData( LOG_COMMAND_TOKEN_TEST_HOST, LOG_MAGIC_TOKEN_SIZE, reception_buffer,
                                               reception_buffer_size, LOG_TIMEOUT_SERIAL_SHORT_RECEIVE_MS );
    if( status != LOGGING_STATUS_OK )
    {
        *host_type = LOGGING_NO_HOST;
        return false;
    }
    else
    {
        if( Logging::IsDemoToken( ( uint8_t* ) reception_buffer, reception_buffer_size ) )
        {
            *host_type = LOGGING_DEMO_HOST;
        }
        else if( Logging::IsFieldTestToken( ( uint8_t* ) reception_buffer, reception_buffer_size ) )
        {
            *host_type = LOGGING_FIELD_TEST_HOST;
        }
        else if( Logging::IsConnectionTestToken( ( uint8_t* ) reception_buffer, reception_buffer_size ) )
        {
            *host_type = LOGGING_CONNECTION_TEST_HOST;
        }
        else
        {
            *host_type = LOGGING_UNKNOWN_HOST;
        }
        return true;
    }
}

bool Logging::IsDemoToken( const uint8_t* buffer, const uint8_t buffer_size )
{
    return Logging::AreBuffersEqual( buffer, buffer_size, ( uint8_t* ) Logging::magic_token_demo,
                                     LOG_MAGIC_TOKEN_SIZE );
}

bool Logging::IsFieldTestToken( const uint8_t* buffer, const uint8_t buffer_size )
{
    return Logging::AreBuffersEqual( buffer, buffer_size, ( uint8_t* ) Logging::magic_token_field_test,
                                     LOG_MAGIC_TOKEN_SIZE );
}

bool Logging::IsConnectionTestToken( const uint8_t* buffer, const uint8_t buffer_size )
{
    return Logging::AreBuffersEqual( buffer, buffer_size, ( uint8_t* ) Logging::magic_token_connection_test,
                                     LOG_MAGIC_TOKEN_SIZE );
}

bool Logging::AreBuffersEqual( const uint8_t* buffer1, const uint8_t buffer_size_1, const uint8_t* buffer2,
                               const uint8_t buffer_size_2 )
{
    if( buffer_size_1 != buffer_size_2 )
    {
        return false;
    }
    for( uint8_t index_buff = 0; index_buff < buffer_size_1; index_buff++ )
    {
        if( buffer1[index_buff] != buffer2[index_buff] )
        {
            return false;
        }
    }
    return true;
}

LoggingStatus_t Logging::ReceiveData( const uint16_t buffer_length_max, char* buffer, uint16_t& buffer_length_received,
                                      const uint16_t timeout )
{
    uint32_t timer_count_ms      = system_time_GetTicker( );
    int      c                   = 0;
    uint16_t local_buffer_length = 0;
    while( true )
    {
        if( system_uart_is_readable( ) )
        {
            c                           = system_uart_receive_char( );
            buffer[local_buffer_length] = c;
            local_buffer_length++;
            if( c == 0 )
            {
                buffer_length_received = local_buffer_length;
                return LOGGING_STATUS_OK;
            }
            timer_count_ms = system_time_GetTicker( );
        }
        if( buffer_length_received >= buffer_length_max )
        {
            buffer_length_received = local_buffer_length;
            return LOGGING_STATUS_OVERFLOW;
        }
        if( ( system_time_GetTicker( ) - timer_count_ms ) > ( timeout ) )
        {
            buffer_length_received = local_buffer_length;
            return LOGGING_STATUS_TIMEOUT;
        }
    }
}

LoggingStatus_t Logging::GetDateAndApproximateLocation( uint32_t& gps_second, float& latitude, float& longitude,
                                                        float& altitude )
{
    if( Logging::logging_enable )
    {
        char            data_loc_blob[64]    = { 0 };
        uint16_t        data_loc_blob_length = 0;
        LoggingStatus_t status = Logging::AskData( LOG_COMMAND_TOKEN_DATE, 64, data_loc_blob, data_loc_blob_length,
                                                   LOG_TIMEOUT_SERIAL_RECEIVE_MS );
        if( status == LOGGING_STATUS_OK )
        {
            sscanf( data_loc_blob, "%u,%f,%f,%f", &gps_second, &altitude, &latitude, &longitude );
        }
        return status;
    }
    else
    {
        return LOGGING_STATUS_DISABLED;
    }
}

LoggingStatus_t Logging::AskData( const char* token, const uint16_t buffer_length_max, char* buffer,
                                  uint16_t& buffer_length_received, const uint16_t timeout )
{
    if( Logging::logging_enable )
    {
        buffer_length_received = 0;
        Logging::FlushSerial( );
        Logging::SendCommand( token );
        system_uart_start_receiving( );
        const LoggingStatus_t status =
            Logging::ReceiveData( buffer_length_max, buffer, buffer_length_received, timeout );
        system_uart_stop_receiving( );
        return status;
    }
    else
    {
        return LOGGING_STATUS_DISABLED;
    }
}

void Logging::SendDataStoredToServer( ) { Logging::SendCommand( LOG_COMMAND_TOKEN_SEND ); }

void Logging::EraseDataStored( ) { Logging::SendCommand( LOG_COMMAND_TOKEN_FLUSH ); }

LoggingStatus_t Logging::GetResults( float& latitude, float& longitude, float& altitude, float& accuracy,
                                     char* geo_coding, const uint8_t geo_coding_max_length )
{
    if( Logging::logging_enable )
    {
        char     reception_buffer[LOG_GET_RESULT_BUFFER_LENGTH] = { 0 };
        char     format[LOG_TMP_FORMAT_LENGTH]                  = { 0 };
        uint16_t reception_buffer_size                          = 0;

        LoggingStatus_t status =
            Logging::AskData( LOG_COMMAND_TOKEN_RESULT, LOG_GET_RESULT_BUFFER_LENGTH, reception_buffer,
                              reception_buffer_size, LOG_TIMEOUT_SERIAL_RECEIVE_MS );
        if( status == LOGGING_STATUS_OK )
        {
            snprintf( format, LOG_TMP_FORMAT_LENGTH, "%%f;%%f;%%f;%%f;%%%d[^\t\n]", geo_coding_max_length - 1 );
            sscanf( reception_buffer, format, &latitude, &longitude, &altitude, &accuracy, geo_coding );
        }
        return status;
    }
    else
    {
        return LOGGING_STATUS_DISABLED;
    }
}

void Logging::SendCommand( const char* command )
{
    if( Logging::logging_enable )
    {
        printf( "!%s\n", command );
    }
}

void Logging::FlushSerial( )
{
    int c = 0;
    while( true )
    {
        if( system_uart_is_readable( ) )
        {
            c = system_uart_receive_char( );
            printf( "# Flush 0x%02x\n", c );
        }
        else
        {
            break;
        }
    }
}
