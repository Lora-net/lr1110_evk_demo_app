/**
 * @file      guiMenuCommon.cpp
 *
 * @brief     Implementation of the gui common menu page.
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

#include "guiMenuCommon.h"

#define BUTTON_HEIGHT 40
#define BUTTON_WIDTH 60
#define LABEL_WIDTH 150
#define MARGIN 10

GuiMenuCommon::GuiMenuCommon( guiPageType_t pageType ) : GuiCommon( pageType ) {}

GuiMenuCommon::~GuiMenuCommon( ) {}

void GuiMenuCommon::createTestEntry( int16_t y_pos, lv_obj_t** lbl, lv_obj_t** btn, lv_obj_t** lbl_btn,
                                     const char* lbl_name, bool is_clickable, lv_event_cb_t event_cb )
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
    lv_obj_set_event_cb( *btn, event_cb );
    lv_obj_set_user_data( *btn, this );
    lv_obj_align( *btn, NULL, LV_ALIGN_IN_RIGHT_MID, -MARGIN, y_pos );

    // Create the label attached to the button
    *lbl_btn = lv_label_create( *btn, NULL );
    lv_label_set_text( *lbl_btn, "GO!" );
}
