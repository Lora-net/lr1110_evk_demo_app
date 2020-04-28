/**
 * @file      command_configure.h
 *
 * @brief     Definitions of the HCI configuration command class.
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

#ifndef __COMMAND_CONFIGURE_H__
#define __COMMAND_CONFIGURE_H__

#include "command_base.h"
#include "demo_configuration.h"
#include "demo.h"

class CommandConfigure : public CommandBase
{
   public:
    CommandConfigure( radio_t* radio, Hci& hci, Demo& demo_holder );
    virtual ~CommandConfigure( );

    virtual uint16_t GetComCode( );
    virtual bool     ConfigureFromPayload( const uint8_t* buffer, const uint16_t buffer_size );
    virtual bool     Job( );

   protected:
    bool ConfigureWifi( const bool enable, const bool use_country_code_api, const uint16_t channel_mask,
                        const uint8_t type_mask, const uint8_t wifi_nbr_retrials, const uint8_t wifi_max_results,
                        const uint16_t wifi_timeout, const lr1110_wifi_mode_t wifi_mode );
    bool ConfigureGnssAutonomous( const bool enable, const uint8_t option, const uint8_t capture_mode,
                                  const uint8_t nb_satellite, const uint8_t antenna_selection,
                                  const uint8_t constellation_mask );
    bool ConfigureGnssAssisted( const bool enable, const uint8_t option, const uint8_t capture_mode,
                                const uint8_t nb_satellite, const uint8_t antenna_selection,
                                const uint8_t constellation_mask );

    static bool ConfigureGnss( demo_gnss_settings_t* setting, const bool enable, const uint8_t option,
                               const uint8_t capture_mode, const uint8_t nb_satellite, const uint8_t antenna_selection,
                               const uint8_t constellation_mask );

   private:
    demo_all_settings_t demo_settings;
    Demo&               demo_holder;
};

#endif  // __COMMAND_CONFIGURE_H__
