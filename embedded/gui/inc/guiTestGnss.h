/**
 * @file      guiTestGnss.h
 *
 * @brief     Definition of the gui test GNSS page.
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

#ifndef __GUI_TEST_GNSS_H__
#define __GUI_TEST_GNSS_H__

#include "guiCommon.h"

class GuiTestGnss : public GuiCommon
{
   public:
    GuiTestGnss( const GuiGnssResult_t* results, guiPageType_t pageType );
    virtual ~GuiTestGnss( );

    virtual void init( );
    virtual void start( );
    virtual void draw( );
    virtual void refresh( );
    virtual void updateHostConnectivityState( void );

    static void callback( lv_obj_t* obj, lv_event_t event );

    lv_obj_t* info_frame;
    lv_obj_t* lbl_info_frame_1;
    lv_obj_t* lbl_info_frame_2;
    lv_obj_t* lbl_info_frame_3;
    lv_obj_t* btn_start_stop;
    lv_obj_t* btn_results;
    lv_obj_t* btn_send;
    lv_obj_t* btn_back;
    lv_obj_t* btn_config;

   private:
    const GuiGnssResult_t* _results;
};

#endif
