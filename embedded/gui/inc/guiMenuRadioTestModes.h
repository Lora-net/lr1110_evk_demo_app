/**
 * @file      guiMenuRadioTestModes.h
 *
 * @brief     Definition of the gui menu radio test page.
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

#ifndef __GUI_MENU_RADIO_TEST_MODES_H__
#define __GUI_MENU_RADIO_TEST_MODES_H__

#include "guiMenuCommon.h"

class GuiMenuRadioTestModes : public GuiMenuCommon
{
   public:
    GuiMenuRadioTestModes( );
    virtual ~GuiMenuRadioTestModes( );

    virtual void draw( );

    static void callback( lv_obj_t* obj, lv_event_t event );

   private:
    lv_obj_t* lbl_radio_tx_cw;
    lv_obj_t* btn_radio_tx_cw;
    lv_obj_t* lbl_btn_radio_tx_cw;
    lv_obj_t* lbl_radio_per;
    lv_obj_t* btn_radio_per;
    lv_obj_t* lbl_btn_radio_per;
    lv_obj_t* lbl_radio_ping_pong;
    lv_obj_t* btn_radio_ping_pong;
    lv_obj_t* lbl_btn_radio_ping_pong;
    lv_obj_t* btn_back;
    lv_obj_t* btn_config;
};

#endif  // __GUI_MENU_RADIO_TEST_MODES_H__
