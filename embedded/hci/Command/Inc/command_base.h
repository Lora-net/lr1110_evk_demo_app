/**
 * @file      command_base.h
 *
 * @brief     Definitions of the HCI command base class.
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

#ifndef __COMMAND_BASE_H__
#define __COMMAND_BASE_H__

#include "command_interface.h"
#include "device_interface.h"
#include "hci.h"

typedef enum
{
    COMMAND_BASE_NO_DEMO                = 0,
    COMMAND_BASE_DEMO_WIFI_SCAN         = 1,
    COMMAND_BASE_DEMO_WIFI_COUNTRY_CODE = 2,
    COMMAND_BASE_DEMO_GNSS_AUTONOMOUS   = 3,
    COMMAND_BASE_DEMO_GNSS_ASSISTED     = 4,
} CommandBaseDemoId_t;

class CommandBase : public CommandInterface
{
   public:
    CommandBase( DeviceInterface* device, Hci& hci );
    virtual ~CommandBase( );

    virtual CommandEvent_t Execute( );

   protected:
    virtual bool     Job( ) { return false; }
    void             SetEventStartDemo( CommandBaseDemoId_t demo_to_start );
    void             SetEventStopDemo( );
    void             SetEventResetDemo( void );
    void             SetNoEvent( );
    void             SendResponseBuffer( const uint8_t* buffer, const uint16_t buffer_length );
    DeviceInterface* device;

   private:
    Hci*           hci;
    CommandEvent_t event;
};

#endif  // __COMMAND_BASE_H__
