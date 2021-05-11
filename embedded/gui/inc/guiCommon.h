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
    GUI_PAGE_EUI,
    GUI_PAGE_MENU,
    GUI_PAGE_CONNECTIVITY,
    GUI_PAGE_MENU_RADIO_TEST_MODES,
    GUI_PAGE_RADIO_TEST_MODES_CONFIG,
    GUI_PAGE_RADIO_TX_CW,
    GUI_PAGE_RADIO_PER,
    GUI_PAGE_RADIO_PING_PONG,
    GUI_PAGE_MENU_DEMO,
    GUI_PAGE_MENU_GEOLOC_DEMO,
    GUI_PAGE_MENU_RADIO_DEMO,
    GUI_PAGE_WIFI_TEST,
    GUI_PAGE_WIFI_RESULTS,
    GUI_PAGE_WIFI_CONFIG,
    GUI_PAGE_GNSS_AUTONOMOUS_TEST,
    GUI_PAGE_GNSS_AUTONOMOUS_RESULTS,
    GUI_PAGE_GNSS_AUTONOMOUS_CONFIG,
    GUI_PAGE_GNSS_ASSISTED_TEST,
    GUI_PAGE_GNSS_ASSISTED_RESULTS,
    GUI_PAGE_GNSS_ASSISTED_CONFIG,
    GUI_PAGE_GNSS_ASSISTANCE_POSITION_CONFIG,
    GUI_PAGE_TEMPERATURE_DEMO,
    GUI_PAGE_FILE_UPLOAD_DEMO,
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
    GUI_EVENT_LAUNCH_CONNECTIVITY,
    GUI_EVENT_EUI,
    GUI_EVENT_LAUNCH_RADIO_TEST_MODE,
    GUI_EVENT_LAUNCH_DEMO,
    GUI_EVENT_LAUNCH_GEOLOC_DEMO,
    GUI_EVENT_LAUNCH_RADIO_DEMO,
    GUI_EVENT_START_TX_CW,
    GUI_EVENT_START_PER_TX,
    GUI_EVENT_START_PER_RX,
    GUI_EVENT_START_PING_PONG,
    GUI_EVENT_START_WIFI,
    GUI_EVENT_START_GNSS_AUTONOMOUS,
    GUI_EVENT_START_GNSS_ASSISTED,
    GUI_EVENT_START_GNSS_ASSISTED_LP,
    GUI_EVENT_START_DEMO_TEMPERATURE,
    GUI_EVENT_START_DEMO_FILE_UPLOAD,
    GUI_EVENT_JOIN,
    GUI_EVENT_ABORT,
    GUI_EVENT_LEAVE,
    GUI_EVENT_ASSISTANCE_POSITION,
    GUI_EVENT_RESTORE_EUI_KEYS,
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

    virtual void refresh( ){ };
    virtual void start( ){ };
    virtual void stop( ){ };

    guiEvent_t    getAndClearEvent( );
    guiPageType_t getType( );

    lv_obj_t* createSection( const char* text, int16_t y_offset_from_center );
    void      createInfoFrame( lv_obj_t** info_frame, lv_obj_t** lbl_info_frame_1, const char* text_1,
                               lv_obj_t** lbl_info_frame_2, const char* text_2, lv_obj_t** lbl_info_frame_3,
                               const char* text_3 );
    lv_obj_t* createActionButton( lv_obj_t** btn, const char* lbl_btn_name, lv_event_cb_t event_cb,
                                  guiButtonPos_t button_pos, int16_t y_pos, bool is_clickable );
    void      createChoiceSwitch( lv_obj_t** sw, lv_obj_t* screen, const char* lbl_sw_name_left,
                                  const char* lbl_sw_name_right, lv_event_cb_t event_cb, int16_t y_pos, bool is_visible );
    void      updateHostConnectivityState( const bool is_connected );
    void      updateCommissioningData( );
    void      updateFakeLedState( bool is_on );
    void      updateNetworkConnectivityState( const GuiNetworkConnectivityStatus_t* new_connectivity_status );
    void      toggleFakeLedState( );

    static float    convertConsoToUah( const uint32_t conso_uas );
    static uint32_t check_value_limits( const uint32_t value, const uint32_t limit_low, const uint32_t limit_high );

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
    static lv_style_t led_on;
    static lv_style_t led_off;

    static guiEvent_t _event;
    static bool       _is_gui_environment_init;
    static bool       _has_connectivity;

   protected:
    void createHeader( const char* text );
    void createHeaderWithIcons( const char* text );
    void createDropDownList( lv_obj_t** ddlist, lv_obj_t* screen, int16_t off_y, const char* lbl_name,
                             const char* options, lv_event_cb_t event_cb, int16_t width, uint16_t selectedOption );

    guiPageType_t                         _pageType;
    lv_obj_t*                             screen;
    lv_obj_t*                             _label_connectivity_icon;
    lv_obj_t*                             _label_fake_led_icon;
    static bool                           _is_host_connected;
    static GuiNetworkConnectivityStatus_t _network_connectivity_status;
    static bool                           _fake_led_state;

   private:
    void         createHeaderIcons( );
    void         createNetworkConnectivityIcon( );
    void         createFakeLedIcon( );
    void         updateNetworkConnectivityIcon( );
    void         updateFakeLedIconState( );
    virtual void propagateHostConnectivityStateChange( ){ };
    virtual void propagateCommissioningChange( ){ };
    virtual void propagateNetworkConnectivityStateChange( ){ };
};

#endif
