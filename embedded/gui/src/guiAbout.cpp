/**
 * @file      guiAbout.cpp
 *
 * @brief     Implementation of the gui about page.
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

#include "guiAbout.h"
#include "version.h"
#include "semtech_logo.h"

#define GUI_ABOUT_BUFFER_LENGTH ( 30 )

GuiAbout::GuiAbout( version_handler_t* version_handler ) : GuiCommon( )
{
    char buffer[GUI_ABOUT_BUFFER_LENGTH];

    this->_pageType = GUI_PAGE_ABOUT;

    this->version_handler = version_handler;

    this->screen = lv_obj_create( NULL, NULL );
    lv_obj_set_style( this->screen, &( GuiCommon::screen_style ) );

    this->createHeader( "ABOUT" );

    snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "LR1110 EVK -  %s", DEMO_VERSION );
    lv_obj_t* lbl_version_evk = lv_label_create( this->screen, NULL );
    lv_obj_set_style( lbl_version_evk, &( GuiCommon::screen_style ) );
    lv_label_set_long_mode( lbl_version_evk, LV_LABEL_LONG_BREAK );
    lv_label_set_align( lbl_version_evk, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( lbl_version_evk, buffer );
    lv_obj_set_width( lbl_version_evk, 240 );
    lv_obj_align( lbl_version_evk, NULL, LV_ALIGN_CENTER, 0, -100 );

    snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "LR1110 TYPE - 0x%02x", this->version_handler->version_chip_type );
    lv_obj_t* lbl_version_type = lv_label_create( this->screen, NULL );
    lv_obj_set_style( lbl_version_type, &( GuiCommon::screen_style ) );
    lv_label_set_long_mode( lbl_version_type, LV_LABEL_LONG_BREAK );
    lv_label_set_align( lbl_version_type, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( lbl_version_type, buffer );
    lv_obj_set_width( lbl_version_type, 240 );
    lv_obj_align( lbl_version_type, NULL, LV_ALIGN_CENTER, 0, -80 );

    snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "LR1110 HARDWARE - 0x%02x", this->version_handler->version_chip_hw );
    lv_obj_t* lbl_version_hw = lv_label_create( this->screen, NULL );
    lv_obj_set_style( lbl_version_hw, &( GuiCommon::screen_style ) );
    lv_label_set_long_mode( lbl_version_hw, LV_LABEL_LONG_BREAK );
    lv_label_set_align( lbl_version_hw, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( lbl_version_hw, buffer );
    lv_obj_set_width( lbl_version_hw, 240 );
    lv_obj_align( lbl_version_hw, NULL, LV_ALIGN_CENTER, 0, -60 );

    snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "LR1110 FIRMWARE - 0x%04x", this->version_handler->version_chip_fw );
    lv_obj_t* lbl_version_fw = lv_label_create( this->screen, NULL );
    lv_obj_set_style( lbl_version_fw, &( GuiCommon::screen_style ) );
    lv_label_set_long_mode( lbl_version_fw, LV_LABEL_LONG_BREAK );
    lv_label_set_align( lbl_version_fw, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( lbl_version_fw, buffer );
    lv_obj_set_width( lbl_version_fw, 240 );
    lv_obj_align( lbl_version_fw, NULL, LV_ALIGN_CENTER, 0, -40 );

    lv_obj_t* lbl_copyrights_lvgl = lv_label_create( this->screen, NULL );
    lv_obj_set_style( lbl_copyrights_lvgl, &( GuiCommon::screen_style ) );
    lv_label_set_long_mode( lbl_copyrights_lvgl, LV_LABEL_LONG_BREAK );
    lv_label_set_align( lbl_copyrights_lvgl, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( lbl_copyrights_lvgl, "LVGL\nCopyright (c) 2016\nGabor Kiss-Vamosi" );
    lv_obj_set_width( lbl_copyrights_lvgl, 240 );
    lv_obj_align( lbl_copyrights_lvgl, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -110 );

    lv_obj_t* lbl_copyrights_osm = lv_label_create( this->screen, NULL );
    lv_obj_set_style( lbl_copyrights_osm, &( GuiCommon::screen_style ) );
    lv_label_set_long_mode( lbl_copyrights_osm, LV_LABEL_LONG_BREAK );
    lv_label_set_align( lbl_copyrights_osm, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( lbl_copyrights_osm, "Data (c) OpenStreetMap\ncontributors, ODbL 1.0." );
    lv_obj_set_width( lbl_copyrights_osm, 240 );
    lv_obj_align( lbl_copyrights_osm, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -60 );

    this->createActionButton( &( this->btn_back ), "BACK", GuiAbout::callback, GUI_BUTTON_POS_CENTER, -5, true );
}

GuiAbout::~GuiAbout( ) { lv_obj_del( this->screen ); }

void GuiAbout::draw( ) { lv_scr_load( this->screen ); }

void GuiAbout::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiAbout* self = ( GuiAbout* ) lv_obj_get_user_data( obj );

    if( event == LV_EVENT_RELEASED )
    {
        if( obj == self->btn_back )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
    }
}
