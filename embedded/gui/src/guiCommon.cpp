/**
 * @file      guiCommon.cpp
 *
 * @brief     Implementation of the gui common page.
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

#include "guiCommon.h"

#define GUI_COMMON_HEADER_HEIGHT 30
#define GUI_COMMON_HEADER_WIDTH 230
#define GUI_COMMON_HEADER_MARGIN_TOP 5

#define GUI_COMMON_DIVIDER_CONSO ( 3600.0 )

#define GUI_COMMON_ACTION_BUTTON_HEIGHT 40
#define GUI_COMMON_ACTION_BUTTON_WIDTH 72
#define MARGIN 5

#define GUI_COMMON_INFOFRAME_HEIGHT 150
#define GUI_COMMON_INFOFRAME_WIDTH 230

bool       GuiCommon::_is_host_connected       = false;
bool       GuiCommon::_is_gui_environment_init = false;
guiEvent_t GuiCommon::_event                   = GUI_EVENT_NONE;
lv_style_t GuiCommon::screen_style;
lv_style_t GuiCommon::note_style;
lv_style_t GuiCommon::title_style;
lv_style_t GuiCommon::info_frame_style_init;
lv_style_t GuiCommon::info_frame_style_ok;
lv_style_t GuiCommon::info_frame_style_ongoing;
lv_style_t GuiCommon::info_frame_style_error;
lv_style_t GuiCommon::sw_knob;
lv_style_t GuiCommon::sw_indic;
lv_style_t GuiCommon::table_cell1;
lv_style_t GuiCommon::tab;

GuiCommon::GuiCommon( guiPageType_t pageType ) : _pageType( pageType )
{
    this->screen = lv_obj_create( NULL, NULL );

    if( GuiCommon::_is_gui_environment_init == false )
    {
        GuiCommon::_is_gui_environment_init = true;

        lv_style_copy( &( GuiCommon::screen_style ), &lv_style_scr );
        GuiCommon::screen_style.body.main_color = LV_COLOR_BLACK;
        GuiCommon::screen_style.body.grad_color = LV_COLOR_BLACK;
        GuiCommon::screen_style.text.color      = LV_COLOR_WHITE;

        lv_style_copy( &( GuiCommon::note_style ), &lv_style_scr );
        GuiCommon::note_style.body.main_color = LV_COLOR_BLACK;
        GuiCommon::note_style.body.grad_color = LV_COLOR_BLACK;
        GuiCommon::note_style.text.color      = LV_COLOR_GRAY;

        lv_style_copy( &( GuiCommon::title_style ), &lv_style_scr );
        GuiCommon::title_style.body.main_color = LV_COLOR_BLACK;
        GuiCommon::title_style.body.grad_color = LV_COLOR_BLACK;
        GuiCommon::title_style.text.color      = LV_COLOR_WHITE;
        GuiCommon::title_style.text.font       = &lv_font_roboto_22;

        lv_style_copy( &( GuiCommon::info_frame_style_init ), &lv_style_scr );
        GuiCommon::info_frame_style_init.body.main_color   = LV_COLOR_BLACK;
        GuiCommon::info_frame_style_init.body.grad_color   = LV_COLOR_BLACK;
        GuiCommon::info_frame_style_init.body.border.color = LV_COLOR_WHITE;
        GuiCommon::info_frame_style_init.body.border.width = 3;
        GuiCommon::info_frame_style_init.text.color        = LV_COLOR_WHITE;

        lv_style_copy( &( GuiCommon::info_frame_style_ok ), &lv_style_scr );
        GuiCommon::info_frame_style_ok.body.main_color   = LV_COLOR_BLACK;
        GuiCommon::info_frame_style_ok.body.grad_color   = LV_COLOR_BLACK;
        GuiCommon::info_frame_style_ok.body.border.color = LV_COLOR_GREEN;
        GuiCommon::info_frame_style_ok.body.border.width = 3;
        GuiCommon::info_frame_style_ok.text.color        = LV_COLOR_GREEN;

        lv_style_copy( &( GuiCommon::info_frame_style_ongoing ), &lv_style_scr );
        GuiCommon::info_frame_style_ongoing.body.main_color   = LV_COLOR_BLACK;
        GuiCommon::info_frame_style_ongoing.body.grad_color   = LV_COLOR_BLACK;
        GuiCommon::info_frame_style_ongoing.body.border.color = LV_COLOR_YELLOW;
        GuiCommon::info_frame_style_ongoing.body.border.width = 3;
        GuiCommon::info_frame_style_ongoing.text.color        = LV_COLOR_YELLOW;

        lv_style_copy( &( GuiCommon::info_frame_style_error ), &lv_style_scr );
        GuiCommon::info_frame_style_error.body.main_color   = LV_COLOR_BLACK;
        GuiCommon::info_frame_style_error.body.grad_color   = LV_COLOR_BLACK;
        GuiCommon::info_frame_style_error.body.border.color = LV_COLOR_RED;
        GuiCommon::info_frame_style_error.body.border.width = 3;
        GuiCommon::info_frame_style_error.text.color        = LV_COLOR_RED;

        lv_style_copy( &( GuiCommon::sw_indic ), &lv_style_pretty_color );
        GuiCommon::sw_indic.body.radius         = LV_RADIUS_CIRCLE;
        GuiCommon::sw_indic.body.main_color     = LV_COLOR_SILVER;
        GuiCommon::sw_indic.body.grad_color     = LV_COLOR_SILVER;
        GuiCommon::sw_indic.body.padding.left   = 0;
        GuiCommon::sw_indic.body.padding.right  = 0;
        GuiCommon::sw_indic.body.padding.top    = 0;
        GuiCommon::sw_indic.body.padding.bottom = 0;

        lv_style_copy( &( GuiCommon::sw_knob ), &lv_style_pretty );
        GuiCommon::sw_knob.body.radius       = LV_RADIUS_CIRCLE;
        GuiCommon::sw_knob.body.shadow.width = 4;
        GuiCommon::sw_knob.body.shadow.type  = LV_SHADOW_BOTTOM;

        lv_style_copy( &( GuiCommon::table_cell1 ), &lv_style_plain );
        GuiCommon::table_cell1.text.color        = LV_COLOR_WHITE;
        GuiCommon::table_cell1.body.border.width = 1;
        GuiCommon::table_cell1.body.border.color = LV_COLOR_WHITE;
        GuiCommon::table_cell1.body.main_color   = LV_COLOR_BLACK;
        GuiCommon::table_cell1.body.grad_color   = LV_COLOR_BLACK;

        lv_style_copy( &( GuiCommon::tab ), &lv_style_btn_rel );
        GuiCommon::tab.body.padding.top    = 10;
        GuiCommon::tab.body.padding.bottom = 10;
    }

    lv_obj_set_style( this->screen, &( GuiCommon::screen_style ) );
}

GuiCommon::~GuiCommon( ) { lv_obj_del( this->screen ); }

guiEvent_t GuiCommon::getAndClearEvent( )
{
    guiEvent_t event = GuiCommon::_event;

    GuiCommon::_event = GUI_EVENT_NONE;

    return event;
}

guiPageType_t GuiCommon::getType( ) { return _pageType; }

void GuiCommon::createHeader( const char* text )
{
    lv_obj_t* header = lv_cont_create( this->screen, NULL );
    lv_obj_set_auto_realign( header, true );
    lv_obj_align( header, NULL, LV_ALIGN_IN_TOP_MID, 0, GUI_COMMON_HEADER_MARGIN_TOP );
    lv_obj_set_height( header, GUI_COMMON_HEADER_HEIGHT );
    lv_obj_set_width( header, GUI_COMMON_HEADER_WIDTH );
    lv_cont_set_fit( header, LV_FIT_NONE );
    lv_cont_set_layout( header, LV_LAYOUT_COL_M );

    lv_obj_t* lbl_header = lv_label_create( header, NULL );
    lv_label_set_text( lbl_header, text );
    lv_obj_align( lbl_header, NULL, LV_ALIGN_CENTER, 0, 0 );
}

void GuiCommon::createSection( const char* text, int16_t y_offfset_from_center )
{
    lv_obj_t* label = lv_label_create( this->screen, NULL );
    lv_obj_set_style( label, &( GuiCommon::screen_style ) );
    lv_label_set_long_mode( label, LV_LABEL_LONG_BREAK );
    lv_label_set_align( label, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( label, text );
    lv_obj_set_width( label, 240 );
    lv_obj_align( label, NULL, LV_ALIGN_CENTER, 0, y_offfset_from_center );
}

void GuiCommon::createInfoFrame( lv_obj_t** info_frame, lv_obj_t** lbl_info_frame_1, const char* text_1,
                                 lv_obj_t** lbl_info_frame_2, const char* text_2, lv_obj_t** lbl_info_frame_3,
                                 const char* text_3 )
{
    *info_frame = lv_cont_create( this->screen, NULL );
    lv_obj_set_auto_realign( *info_frame, true );
    lv_obj_align( *info_frame, NULL, LV_ALIGN_CENTER, 0, -30 );
    lv_obj_set_height( *info_frame, GUI_COMMON_INFOFRAME_HEIGHT );
    lv_obj_set_width( *info_frame, GUI_COMMON_INFOFRAME_WIDTH );
    lv_cont_set_fit( *info_frame, LV_FIT_NONE );
    lv_cont_set_layout( *info_frame, LV_LAYOUT_CENTER );
    lv_cont_set_style( *info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_ongoing ) );

    *lbl_info_frame_1 = lv_label_create( *info_frame, NULL );
    lv_label_set_align( *lbl_info_frame_1, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( *lbl_info_frame_1, text_1 );

    *lbl_info_frame_2 = lv_label_create( *info_frame, NULL );
    lv_label_set_align( *lbl_info_frame_2, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( *lbl_info_frame_2, text_2 );

    *lbl_info_frame_3 = lv_label_create( *info_frame, NULL );
    lv_label_set_align( *lbl_info_frame_3, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( *lbl_info_frame_3, text_3 );
}

void GuiCommon::createActionButton( lv_obj_t** btn, const char* lbl_btn_name, lv_event_cb_t event_cb,
                                    guiButtonPos_t button_pos, int16_t y_pos, bool is_clickable )
{
    // Create the button
    *btn = lv_btn_create( this->screen, NULL );
    lv_btn_set_state( *btn, ( is_clickable == true ) ? LV_BTN_STATE_REL : LV_BTN_STATE_INA );
    lv_obj_set_height( *btn, GUI_COMMON_ACTION_BUTTON_HEIGHT );
    lv_obj_set_width( *btn, GUI_COMMON_ACTION_BUTTON_WIDTH );
    lv_obj_set_event_cb( *btn, event_cb );
    lv_obj_set_user_data( *btn, this );

    switch( button_pos )
    {
    case GUI_BUTTON_POS_CENTER:
        lv_obj_align( *btn, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, y_pos );
        break;
    case GUI_BUTTON_POS_LEFT:
        lv_obj_align( *btn, NULL, LV_ALIGN_IN_BOTTOM_LEFT, MARGIN, y_pos );
        break;
    case GUI_BUTTON_POS_RIGHT:
        lv_obj_align( *btn, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -MARGIN, y_pos );
        break;
    }

    // Create the label attached to the button
    lv_obj_t* lbl_btn = lv_label_create( *btn, NULL );
    lv_label_set_text( lbl_btn, lbl_btn_name );
}

void GuiCommon::createChoiceSwitch( lv_obj_t** sw, lv_obj_t* screen, const char* lbl_sw_name_left,
                                    const char* lbl_sw_name_right, lv_event_cb_t event_cb, int16_t y_pos,
                                    bool is_visible )
{
    *sw = lv_sw_create( screen, NULL );
    lv_sw_set_style( *sw, LV_SW_STYLE_INDIC, &( GuiCommon::sw_indic ) );
    lv_sw_set_style( *sw, LV_SW_STYLE_KNOB_ON, &( GuiCommon::sw_knob ) );
    lv_sw_set_style( *sw, LV_SW_STYLE_KNOB_OFF, &( GuiCommon::sw_knob ) );
    lv_obj_align( *sw, NULL, LV_ALIGN_CENTER, 0, y_pos );
    lv_obj_set_event_cb( *sw, event_cb );
    lv_obj_set_user_data( *sw, this );

    if( is_visible == false )
    {
        lv_obj_set_hidden( *sw, true );
    }
    else
    {
        lv_obj_t* lbl_sw_left = lv_label_create( screen, NULL );
        lv_obj_set_style( lbl_sw_left, &( GuiCommon::screen_style ) );
        lv_label_set_align( lbl_sw_left, LV_LABEL_ALIGN_LEFT );
        lv_label_set_text( lbl_sw_left, lbl_sw_name_left );
        lv_obj_set_width( lbl_sw_left, 240 );
        lv_obj_align( lbl_sw_left, NULL, LV_ALIGN_IN_LEFT_MID, 5, y_pos );

        lv_obj_t* lbl_sw_right = lv_label_create( screen, NULL );
        lv_obj_set_style( lbl_sw_right, &( GuiCommon::screen_style ) );
        lv_label_set_align( lbl_sw_right, LV_LABEL_ALIGN_RIGHT );
        lv_label_set_text( lbl_sw_right, lbl_sw_name_right );
        lv_obj_set_width( lbl_sw_right, 240 );
        lv_obj_align( lbl_sw_right, NULL, LV_ALIGN_IN_RIGHT_MID, -5, y_pos );
    }
}

void GuiCommon::updateHostConnectivityState( const bool is_connected )
{
    GuiCommon::_is_host_connected = is_connected;
    this->updateHostConnectivityState( );
}

guiEvent_t GuiCommon::touchEvent( uint16_t x, uint16_t y )
{
    guiEvent_t event = GUI_EVENT_NONE;

    return event;
}

float GuiCommon::convertConsoToUah( const uint32_t conso_uas ) { return conso_uas / GUI_COMMON_DIVIDER_CONSO; }

uint32_t GuiCommon::check_value_limits( const uint32_t value, const uint32_t limit_low, const uint32_t limit_high )
{
    uint32_t in_limit_value = value;

    if( value < limit_low )
    {
        in_limit_value = limit_low;
    }
    else if( in_limit_value > limit_high )
    {
        in_limit_value = limit_high;
    }

    return in_limit_value;
}
