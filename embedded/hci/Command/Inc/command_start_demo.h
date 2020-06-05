/**
 * @file      command_start_demo.h
 *
 * @brief     Definitions of the HCI command to start demo class.
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

#ifndef __COMMAND_START_DEMO_H__
#define __COMMAND_START_DEMO_H__

#include "command_base.h"
#include "hci.h"
#include "demo.h"

class CommandStartDemo : public CommandBase
{
   public:
    explicit CommandStartDemo( DeviceBase* device, Hci& hci, Demo& demo_holder );
    virtual ~CommandStartDemo( );

    virtual uint16_t GetComCode( );
    virtual bool     ConfigureFromPayload( const uint8_t* buffer, const uint16_t buffer_size );
    virtual bool     Job( );

   protected:
    bool ConfigureWifiScan( const uint8_t* buffer, const uint16_t buffer_size );
    bool ConfigureWifiCountryCode( const uint8_t* buffer, const uint16_t buffer_size );
    bool ConfigureGnssAutonomous( const uint8_t* buffer, const uint16_t buffer_size );
    bool ConfigureGnssAssisted( const uint8_t* buffer, const uint16_t buffer_size );
    bool ConfigureGnss( demo_gnss_settings_t* gnss_setting, const uint8_t* buffer, const uint16_t buffer_size );

   private:
    CommandBaseDemoId_t demo_id_to_start;
    demo_all_settings_t demo_settings;
    Demo&               demo_holder;
};

#endif  // __COMMAND_START_DEMO_H__
