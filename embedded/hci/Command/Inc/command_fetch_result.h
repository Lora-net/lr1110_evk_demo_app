/**
 * @file      command_fetch_result.h
 *
 * @brief     Definitions of the HCI command to fetch scan results class.
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

#ifndef __COMMAND_FETCH_RESULT_H__
#define __COMMAND_FETCH_RESULT_H__

#include "command_interface.h"
#include "hci.h"
#include "demo_manager_interface.h"

class CommandFetchResult : public CommandInterface
{
   public:
    CommandFetchResult( Hci& hci, EnvironmentInterface& environment, DemoManagerInterface& demo_holder );
    virtual ~CommandFetchResult( );

    virtual uint16_t       GetComCode( );
    virtual bool           ConfigureFromPayload( const uint8_t* buffer, const uint16_t buffer_size );
    virtual CommandEvent_t Execute( );

   protected:
    void FetchWifiResults( const demo_wifi_scan_all_results_t& wifi_results );
    void FetchAutonomousGnssResults( const demo_gnss_all_results_t& gnss_autonomous_results );
    void FetchAssistedGnssResults( const demo_gnss_all_results_t& gnss_assisted_results );

    void SendGnssResult( const demo_gnss_all_results_t& gnss_result, const uint16_t resp_code );

    /*!
     * \brief Append data to a buffer
     *
     * \param [in] array Pointer to the start of the array to add the value to
     *
     * \param [in] index Offset from the start of the array where to add the
     * value
     *
     * \param [in] value The value to add to buffer
     *
     * \retval The number of bytes written
     */
    static uint8_t AppendValueAtIndex( uint8_t* array, const uint16_t index, const uint32_t value );
    static uint8_t AppendValueAtIndex( uint8_t* array, const uint16_t index, const uint16_t value );
    static uint8_t AppendValueAtIndex( uint8_t* array, const uint16_t index, const uint8_t value );

    static uint8_t ConvertWifiTypeToSerial( const demo_wifi_signal_type_t& wifi_result_type );

    static uint8_t ConvertGnssConstellationToSerial( const demo_gnss_constellation_t& constellation );

   private:
    Hci&                  hci;
    EnvironmentInterface& environment;
    DemoManagerInterface& demo_holder;
};

#endif  // __COMMAND_FETCH_RESULT_H__
