/**
 * @file      guiMenuConnectivity.h
 *
 * @brief     Definition of the GUI connectivity page
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

#ifndef __GUI_CONNECTIVITY_H__
#define __GUI_CONNECTIVITY_H__

#include "guiCommon.h"

class GuiConnectivity : public GuiCommon
{
   public:
    GuiConnectivity( GuiNetworkConnectivitySettings_t* network_connectivity_settings );
    virtual ~GuiConnectivity( );

    void propagateNetworkConnectivityStateChange( ) override;

    static void callback( lv_obj_t* obj, lv_event_t event );
    static void callback_ddlist( lv_obj_t* obj, lv_event_t event );

   private:
    void                              updateButtons( );
    GuiNetworkConnectivitySettings_t* _network_connectivity_settings;
    lv_obj_t*                         btn_back;
    lv_obj_t*                         btn_join;
    lv_obj_t*                         lbl_btn_join;
    lv_obj_t*                         lbl_connectivity_state;
    lv_obj_t*                         ddlist_lorawan_region;
    lv_obj_t*                         ddlist_adr_profile;
    lv_obj_t*                         ddlist_lorawan_class;
};

#endif  // __GUI_CONNECTIVITY_H__
