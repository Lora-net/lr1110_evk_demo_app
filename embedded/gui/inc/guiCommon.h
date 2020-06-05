/**
 * @file      guiCommon.h
 *
 * @brief     Definition of the gui common page.
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

#ifndef __GUI_COMMON_H__
#define __GUI_COMMON_H__

#include "guiFormat.h"
#include <string.h>
#include "stdio.h"
#include "lvgl.h"

#define GUI_LABEL_BUTTON_START "START"
#define GUI_LABEL_BUTTON_STOP "STOP"
#define GUI_LABEL_BUTTON_RESULTS "RESULTS"
#define GUI_LABEL_BUTTON_SEND "SEND"
#define GUI_LABEL_BUTTON_BACK "BACK"

typedef enum
{
    GUI_PAGE_NONE = 0,
    GUI_PAGE_SPLASHSCREEN,
    GUI_PAGE_ABOUT,
    GUI_PAGE_MENU,
    GUI_PAGE_MENU_RADIO_TEST_MODES,
    GUI_PAGE_RADIO_TEST_MODES_CONFIG,
    GUI_PAGE_RADIO_TX_CW,
    GUI_PAGE_RADIO_PER,
    GUI_PAGE_RADIO_PING_PONG,
    GUI_PAGE_MENU_DEMO,
    GUI_PAGE_WIFI_TEST,
    GUI_PAGE_WIFI_RESULTS,
    GUI_PAGE_WIFI_CONFIG,
    GUI_PAGE_GNSS_AUTONOMOUS_TEST,
    GUI_PAGE_GNSS_AUTONOMOUS_RESULTS,
    GUI_PAGE_GNSS_AUTONOMOUS_CONFIG,
    GUI_PAGE_GNSS_ASSISTED_TEST,
    GUI_PAGE_GNSS_ASSISTED_RESULTS,
    GUI_PAGE_GNSS_ASSISTED_CONFIG,
} guiPageType_t;

typedef enum
{
    GUI_EVENT_NONE = 0,
    GUI_EVENT_NEXT,
    GUI_EVENT_BACK,
    GUI_EVENT_LEFT,
    GUI_EVENT_RIGHT,
    GUI_EVENT_RESULTS,
    GUI_EVENT_CONFIG,
    GUI_EVENT_ABOUT,
    GUI_EVENT_SAVE,
    GUI_EVENT_SEND,
    GUI_EVENT_STOP,
    GUI_EVENT_LAUNCH_RADIO_TEST_MODE,
    GUI_EVENT_LAUNCH_DEMO,
    GUI_EVENT_START_TX_CW,
    GUI_EVENT_START_PER_TX,
    GUI_EVENT_START_PER_RX,
    GUI_EVENT_START_PING_PONG,
    GUI_EVENT_START_WIFI,
    GUI_EVENT_START_GNSS_AUTONOMOUS,
    GUI_EVENT_START_GNSS_ASSISTED,
    GUI_EVENT_START_GNSS_ASSISTED_LP,
} guiEvent_t;

typedef enum
{
    GUI_BUTTON_POS_CENTER = 0,
    GUI_BUTTON_POS_LEFT,
    GUI_BUTTON_POS_RIGHT,
} guiButtonPos_t;

class GuiCommon
{
   public:
    GuiCommon( guiPageType_t pageType );
    virtual ~GuiCommon( );

    virtual void       init( ){};
    virtual void       draw( ){};
    virtual void       refresh( ){};
    virtual guiEvent_t getAndClearEvent( );
    virtual void       updateHostConnectivityState( ){};
    virtual void       start( ){};
    virtual void       stop( ){};
    virtual void       updateResults( guiEvent_t event ){};

    guiPageType_t getType( );

    void createSection( const char* text, int16_t y_offfset_from_center );
    void createInfoFrame( lv_obj_t** info_frame, lv_obj_t** lbl_info_frame_1, const char* text_1,
                          lv_obj_t** lbl_info_frame_2, const char* text_2, lv_obj_t** lbl_info_frame_3,
                          const char* text_3 );
    void createActionButton( lv_obj_t** btn, const char* lbl_btn_name, lv_event_cb_t event_cb,
                             guiButtonPos_t button_pos, int16_t y_pos, bool is_clickable );
    void createChoiceSwitch( lv_obj_t** sw, lv_obj_t* screen, const char* lbl_sw_name_left,
                             const char* lbl_sw_name_right, lv_event_cb_t event_cb, int16_t y_pos, bool is_visible );
    void updateHostConnectivityState( const bool is_connected );

    static float    convertConsoToUah( const uint32_t conso_uas );
    static uint32_t check_value_limits( const uint32_t value, const uint32_t limit_low, const uint32_t limit_high );

    void       createHeader( const char* text );
    guiEvent_t touchEvent( uint16_t x, uint16_t y );

    static lv_style_t screen_style;
    static lv_style_t note_style;
    static lv_style_t title_style;
    static lv_style_t info_frame_style_init;
    static lv_style_t info_frame_style_ok;
    static lv_style_t info_frame_style_ongoing;
    static lv_style_t info_frame_style_error;
    static lv_style_t sw_indic;
    static lv_style_t sw_knob;
    static lv_style_t table_cell1;
    static lv_style_t tab;

    static guiEvent_t _event;
    static bool       _is_gui_environment_init;

   protected:
    guiPageType_t _pageType;
    lv_obj_t*     screen;
    static bool   _is_host_connected;
};

#endif
