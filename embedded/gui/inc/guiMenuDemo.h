/**
 * @file      guiMenuDemo.h
 *
 * @brief     Definition of the gui demo menu page.
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

#ifndef __GUI_MENU_DEMO_H__
#define __GUI_MENU_DEMO_H__

#include "guiCommon.h"

class GuiMenuDemo : public GuiCommon
{
   public:
    GuiMenuDemo( );
    virtual ~GuiMenuDemo( );

    virtual void draw( );
    virtual void updateHostConnectivityState( );
    void createTestEntry( int16_t y_pos, lv_obj_t** lbl, lv_obj_t** btn, lv_obj_t** lbl_btn, const char* lbl_name,
                          bool is_clickable );
    static void callback( lv_obj_t* obj, lv_event_t event );

   private:
    lv_obj_t* lbl_wifi;
    lv_obj_t* btn_wifi;
    lv_obj_t* lbl_btn_wifi;
    lv_obj_t* lbl_gnss_autonomous;
    lv_obj_t* btn_gnss_autonomous;
    lv_obj_t* lbl_btn_gnss_autonomous;
    lv_obj_t* lbl_gnss_assisted;
    lv_obj_t* btn_gnss_assisted;
    lv_obj_t* lbl_btn_gnss_assisted;
    lv_obj_t* btn_back;
    lv_obj_t* lbl_connectivity;
};

#endif
