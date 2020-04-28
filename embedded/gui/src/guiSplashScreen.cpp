/**
 * @file      guiSplashScreen.cpp
 *
 * @brief     Implementation of the gui splash screen page.
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

#include "guiSplashScreen.h"
#include "version.h"
#include "semtech_logo.h"

GuiSplashScreen::GuiSplashScreen( ) : GuiCommon( )
{
    this->_pageType = GUI_PAGE_SPLASHSCREEN;

    this->screen = lv_obj_create( NULL, NULL );
    lv_obj_set_style( this->screen, &( GuiCommon::screen_style ) );

    this->icon = lv_img_create( this->screen, NULL );
    lv_obj_align( this->icon, NULL, LV_ALIGN_IN_TOP_LEFT, 17, 0 );
    lv_img_set_src( this->icon, &semtech_logo );

    lv_obj_t* lbl_title = lv_label_create( this->screen, NULL );
    lv_obj_set_style( lbl_title, &( GuiCommon::title_style ) );
    lv_label_set_long_mode( lbl_title, LV_LABEL_LONG_BREAK );
    lv_label_set_align( lbl_title, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( lbl_title, "LR1110\nEVALUATION KIT" );
    lv_obj_set_width( lbl_title, 240 );
    lv_obj_align( lbl_title, NULL, LV_ALIGN_CENTER, 0, 0 );

    this->lbl_version = lv_label_create( screen, NULL );
    lv_obj_set_style( this->lbl_version, &( GuiCommon::screen_style ) );
    lv_label_set_long_mode( this->lbl_version, LV_LABEL_LONG_BREAK );
    lv_label_set_align( this->lbl_version, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( this->lbl_version, DEMO_VERSION );
    lv_obj_set_width( this->lbl_version, 240 );
    lv_obj_align( this->lbl_version, NULL, LV_ALIGN_CENTER, 0, 40 );

    this->createActionButton( &( this->btn_continue ), "START", GuiSplashScreen::callback, GUI_BUTTON_POS_RIGHT, -5,
                              true );

    this->createActionButton( &( this->btn_about ), "ABOUT", GuiSplashScreen::callback, GUI_BUTTON_POS_LEFT, -5, true );
}

GuiSplashScreen::~GuiSplashScreen( ) { lv_obj_del( this->screen ); }

void GuiSplashScreen::draw( ) { lv_scr_load( this->screen ); }

void GuiSplashScreen::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiSplashScreen* self = ( GuiSplashScreen* ) lv_obj_get_user_data( obj );

    if( event == LV_EVENT_RELEASED )
    {
        if( obj == self->btn_continue )
        {
            GuiCommon::_event = GUI_EVENT_NEXT;
        }
        else if( obj == self->btn_about )
        {
            GuiCommon::_event = GUI_EVENT_ABOUT;
        }
    }
}
