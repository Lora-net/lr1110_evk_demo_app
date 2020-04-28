/**
 * @file      guiMenu.cpp
 *
 * @brief     Implementation of the gui menu page.
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

#include "guiMenu.h"

#define BUTTON_HEIGHT 40
#define BUTTON_WIDTH 60
#define LABEL_WIDTH 150
#define MARGIN 10

GuiMenu::GuiMenu( ) : GuiCommon( )
{
    _pageType = GUI_PAGE_MENU;

    this->screen = lv_obj_create( NULL, NULL );
    lv_obj_set_style( this->screen, &( GuiCommon::screen_style ) );

    this->createHeader( "MENU" );

    this->createTestEntry( -70, &( this->lbl_radio_test_modes ), &( this->btn_radio_test_modes ),
                           &( this->lbl_btn_radio_test_modes ), "RADIO TEST MODES", true );

    this->createTestEntry( -0, &( this->lbl_demos ), &( this->btn_demos ), &( this->lbl_btn_demos ), "DEMONSTRATIONS",
                           true );
}

GuiMenu::~GuiMenu( ) { lv_obj_del( this->screen ); }

void GuiMenu::draw( ) { lv_scr_load( this->screen ); }

void GuiMenu::createTestEntry( int16_t y_pos, lv_obj_t** lbl, lv_obj_t** btn, lv_obj_t** lbl_btn, const char* lbl_name,
                               bool is_clickable )
{
    // Create the label
    *lbl = lv_label_create( this->screen, NULL );
    lv_obj_set_style( *lbl, &( GuiCommon::screen_style ) );
    lv_label_set_long_mode( *lbl, LV_LABEL_LONG_BREAK );
    lv_label_set_align( *lbl, LV_LABEL_ALIGN_LEFT );
    lv_label_set_text( *lbl, lbl_name );
    lv_obj_set_width( *lbl, LABEL_WIDTH );
    lv_obj_align( *lbl, NULL, LV_ALIGN_IN_LEFT_MID, MARGIN, y_pos );

    // Create the button
    *btn = lv_btn_create( this->screen, NULL );
    lv_btn_set_state( *btn, ( is_clickable == true ) ? LV_BTN_STATE_REL : LV_BTN_STATE_INA );
    lv_obj_set_height( *btn, BUTTON_HEIGHT );
    lv_obj_set_width( *btn, BUTTON_WIDTH );
    lv_obj_set_event_cb( *btn, GuiMenu::callback );
    lv_obj_set_user_data( *btn, this );
    lv_obj_align( *btn, NULL, LV_ALIGN_IN_RIGHT_MID, -MARGIN, y_pos );

    // Create the label attached to the button
    *lbl_btn = lv_label_create( *btn, NULL );
    lv_label_set_text( *lbl_btn, "GO!" );
}

void GuiMenu::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiMenu* self = ( GuiMenu* ) lv_obj_get_user_data( obj );

    if( ( event == LV_EVENT_RELEASED ) && ( lv_btn_get_state( obj ) != LV_BTN_STATE_INA ) )
    {
        if( obj == self->btn_radio_test_modes )
        {
            GuiCommon::_event = GUI_EVENT_LAUNCH_RADIO_TEST_MODE;
        }
        else if( obj == self->btn_demos )
        {
            GuiCommon::_event = GUI_EVENT_LAUNCH_DEMO;
        }
    }
}
