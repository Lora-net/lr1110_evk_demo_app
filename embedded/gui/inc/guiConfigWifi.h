/**
 * @file      guiConfigWifi.h
 *
 * @brief     Definition of the gui Wi-Fi config page.
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

#ifndef __GUI_CONFIG_WIFI_H__
#define __GUI_CONFIG_WIFI_H__

#include "guiCommon.h"

class GuiConfigWifi : public GuiCommon
{
   public:
    GuiConfigWifi( GuiWifiDemoSetting_t* settings_current, const GuiWifiDemoSetting_t* settings_default );
    virtual ~GuiConfigWifi( );

    static void callback( lv_obj_t* obj, lv_event_t event );
    static void callbackSettings( lv_obj_t* obj, lv_event_t event );

    bool IsConfigTempEqualTo( const GuiWifiDemoSetting_t* settings_to_compare );

    void ConfigSettingsButton( );
    void ConfigActionButton( );

    lv_obj_t* btn_cancel;
    lv_obj_t* btn_default;
    lv_obj_t* btn_save;
    lv_obj_t* btnm_types;
    lv_obj_t* btnm_channels;

    GuiWifiDemoSetting_t        settings_temp;
    GuiWifiDemoSetting_t*       settings_current;
    const GuiWifiDemoSetting_t* settings_default;

   private:
    const char* btnm_channels_map[18] = { "1", "2",  "3",  "4",  "5",  "\n", "6",  "7",  "8",
                                          "9", "10", "\n", "11", "12", "13", "14", "15", "" };
    const char* btnm_types_map[5]     = { "B", "G", "N", "ALL", "" };
};

#endif
