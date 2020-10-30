/**
 * @file      guiConfigGnssAssistancePosition.h
 *
 * @brief     Definition of the GNSS assistance position configuration page.
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

#ifndef __GUI_CONFIG_GNSS_ASSISTANCE_POSITION_H__
#define __GUI_CONFIG_GNSS_ASSISTANCE_POSITION_H__

#include "guiCommon.h"

class GuiConfigGnssAssistancePosition : public GuiCommon
{
   public:
    GuiConfigGnssAssistancePosition( GuiGnssDemoAssistancePosition_t*       settings,
                                     const GuiGnssDemoAssistancePosition_t* settings_default );
    virtual ~GuiConfigGnssAssistancePosition( );

    static void callback( lv_obj_t* obj, lv_event_t event );
    static void callback_ta( lv_obj_t* obj, lv_event_t event );
    static void callback_kb( lv_obj_t* obj, lv_event_t event );

    bool IsConfigTempEqualTo( const GuiGnssDemoAssistancePosition_t* settings_to_compare );

    void ConfigActionButton( );
    void ConfigParam( );

    GuiGnssDemoAssistancePosition_t        settings_temp;
    GuiGnssDemoAssistancePosition_t*       settings_current;
    const GuiGnssDemoAssistancePosition_t* settings_default;

    char ta_text[10];

   private:
    void      create_ta( lv_obj_t** ta, lv_obj_t* screen, int16_t off_y, const char* lbl_name, uint16_t max_length,
                         const char* init_text, lv_event_cb_t event_cb );
    lv_obj_t* ta_latitude;
    lv_obj_t* ta_longitude;
    lv_obj_t* kb_num;
    lv_obj_t* btn_cancel;
    lv_obj_t* btn_default;
    lv_obj_t* btn_save;
    lv_obj_t* lbl_set_server;
};

#endif  // __GUI_CONFIG_GNSS_ASSISTANCE_POSITION_H__
