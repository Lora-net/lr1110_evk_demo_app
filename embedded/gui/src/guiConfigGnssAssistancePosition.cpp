/**
 * @file      guiConfigGnssAssistancePosition.cpp
 *
 * @brief     Implementation of the gui config assistance position GNSS page.
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

#include "guiConfigGnssAssistancePosition.h"
#include <stdlib.h>

GuiConfigGnssAssistancePosition::GuiConfigGnssAssistancePosition(
    GuiGnssDemoAssistancePosition_t* settings, const GuiGnssDemoAssistancePosition_t* settings_default )
    : GuiCommon( GUI_PAGE_GNSS_ASSISTANCE_POSITION_CONFIG ),
      settings_current( settings ),
      settings_default( settings_default )
{
    this->settings_temp = *( this->settings_current );

    this->createHeaderWithIcons( "GNSS ASSISTANCE" );

    char str[12];
    snprintf( str, 10, "%f", this->settings_temp.latitude );
    this->create_ta( &( this->ta_latitude ), this->screen, 45, "Latitude", 9, str,
                     GuiConfigGnssAssistancePosition::callback_ta );
    snprintf( str, 10, "%f", this->settings_temp.longitude );
    this->create_ta( &( this->ta_longitude ), this->screen, 80, "Longitude", 9, str,
                     GuiConfigGnssAssistancePosition::callback_ta );

    this->createActionButton( &( this->btn_cancel ), "CANCEL", GuiConfigGnssAssistancePosition::callback,
                              GUI_BUTTON_POS_LEFT, -5, true );

    this->createActionButton( &( this->btn_default ), "DEFAULT", GuiConfigGnssAssistancePosition::callback,
                              GUI_BUTTON_POS_CENTER, -5,
                              ( this->IsConfigTempEqualTo( this->settings_default ) == true ) ? false : true );

    this->createActionButton( &( this->btn_save ), "SAVE", GuiConfigGnssAssistancePosition::callback,
                              GUI_BUTTON_POS_RIGHT, -5,
                              ( this->IsConfigTempEqualTo( this->settings_current ) == true ) ? false : true );

    this->lbl_set_server = lv_label_create( this->screen, NULL );
    lv_obj_set_style( this->lbl_set_server, &( GuiCommon::note_style ) );
    lv_label_set_long_mode( this->lbl_set_server, LV_LABEL_LONG_BREAK );
    lv_label_set_align( this->lbl_set_server, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( this->lbl_set_server, "Coordinates set by the server" );

    lv_obj_set_width( this->lbl_set_server, 230 );
    lv_obj_align( this->lbl_set_server, NULL, LV_ALIGN_CENTER, 0, -20 );
    lv_obj_set_hidden( this->lbl_set_server, ( this->settings_temp.set_by_network == false ) ? true : false );

    /*Create a keyboard and apply the styles*/
    this->kb_num = lv_kb_create( this->screen, NULL );
    lv_kb_set_mode( this->kb_num, LV_KB_MODE_NUM );
    lv_kb_set_cursor_manage( this->kb_num, true );
    lv_kb_set_style( this->kb_num, LV_KB_STYLE_BG, &lv_style_transp_tight );
    lv_obj_set_event_cb( this->kb_num, GuiConfigGnssAssistancePosition::callback_kb );
    lv_obj_set_user_data( this->kb_num, this );
    lv_obj_move_foreground( this->kb_num );
    lv_obj_set_hidden( this->kb_num, true );

    lv_scr_load( this->screen );
}

GuiConfigGnssAssistancePosition::~GuiConfigGnssAssistancePosition( ) {}

void GuiConfigGnssAssistancePosition::ConfigActionButton( )
{
    lv_btn_set_state( this->btn_default, ( ( this->IsConfigTempEqualTo( this->settings_default ) == true ) )
                                             ? LV_BTN_STATE_INA
                                             : LV_BTN_STATE_REL );

    lv_btn_set_state( this->btn_save, ( ( this->IsConfigTempEqualTo( this->settings_current ) == true ) )
                                          ? LV_BTN_STATE_INA
                                          : LV_BTN_STATE_REL );

    if( this->IsConfigTempEqualTo( this->settings_current ) == false )  // Settings have been changed manually
    {
        this->settings_temp.set_by_network = false;
        lv_obj_set_hidden( this->lbl_set_server, ( this->settings_temp.set_by_network == false ) ? true : false );
    }
}

void GuiConfigGnssAssistancePosition::ConfigParam( )
{
    char str[10];

    snprintf( str, 5, "%f", this->settings_temp.latitude );
    lv_ta_set_text( this->ta_latitude, str );

    snprintf( str, 5, "%f", this->settings_temp.longitude );
    lv_ta_set_text( this->ta_longitude, str );
}

bool GuiConfigGnssAssistancePosition::IsConfigTempEqualTo( const GuiGnssDemoAssistancePosition_t* settings_to_compare )
{
    if( this->settings_temp.latitude != settings_to_compare->latitude )
    {
        return false;
    }

    if( this->settings_temp.longitude != settings_to_compare->longitude )
    {
        return false;
    }

    return true;
}

void GuiConfigGnssAssistancePosition::create_ta( lv_obj_t** ta, lv_obj_t* screen, int16_t off_y, const char* lbl_name,
                                                 uint16_t max_length, const char* init_text, lv_event_cb_t event_cb )
{
    lv_obj_t* lbl;

    lbl = lv_label_create( screen, NULL );
    lv_obj_set_style( lbl, &( GuiCommon::screen_style ) );
    lv_label_set_text( lbl, lbl_name );
    lv_obj_align( lbl, NULL, LV_ALIGN_IN_TOP_LEFT, 5, off_y );

    *ta = lv_ta_create( screen, NULL );
    lv_obj_set_click( *ta, true );
    lv_obj_set_event_cb( *ta, event_cb );
    lv_obj_set_width( *ta, 100 );
    lv_obj_align( *ta, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, off_y );
    lv_ta_set_accepted_chars( *ta, "0123456789.-" );
    lv_ta_set_cursor_type( *ta, LV_CURSOR_LINE | LV_CURSOR_HIDDEN );
    lv_ta_set_one_line( *ta, true );
    lv_ta_set_max_length( *ta, max_length );
    lv_ta_set_text( *ta, init_text );
    lv_ta_set_text_align( *ta, LV_LABEL_ALIGN_RIGHT );
    lv_obj_set_user_data( *ta, this );
}

void GuiConfigGnssAssistancePosition::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiConfigGnssAssistancePosition* self = ( GuiConfigGnssAssistancePosition* ) lv_obj_get_user_data( obj );

    if( ( event == LV_EVENT_RELEASED ) && ( lv_btn_get_state( obj ) != LV_BTN_STATE_INA ) )
    {
        if( obj == self->btn_cancel )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
        else if( obj == self->btn_default )
        {
            self->settings_temp = *( self->settings_default );
            self->ConfigActionButton( );
            self->ConfigParam( );
        }
        else if( obj == self->btn_save )
        {
            *self->settings_current = self->settings_temp;
            GuiCommon::_event       = GUI_EVENT_SAVE;
        }
    }
}

void GuiConfigGnssAssistancePosition::callback_ta( lv_obj_t* obj, lv_event_t event )
{
    GuiConfigGnssAssistancePosition* self = ( GuiConfigGnssAssistancePosition* ) lv_obj_get_user_data( obj );

    if( event == LV_EVENT_PRESSED )
    {
        if( lv_kb_get_ta( self->kb_num ) == NULL )
        {
            /*Assign the text area to the keyboard*/
            lv_kb_set_ta( self->kb_num, obj );
            lv_obj_set_hidden( self->kb_num, false );

            const char* txt = lv_ta_get_text( obj );
            strcpy( self->ta_text, txt );
        }
    }
}

void GuiConfigGnssAssistancePosition::callback_kb( lv_obj_t* obj, lv_event_t event )
{
    lv_obj_t* ta = lv_kb_get_ta( obj );

    GuiConfigGnssAssistancePosition* self = ( GuiConfigGnssAssistancePosition* ) lv_obj_get_user_data( obj );

    lv_kb_def_event_cb( obj, event );

    if( event == LV_EVENT_CANCEL )
    {
        lv_kb_set_ta( obj, NULL ); /*De-assign the text area*/
        lv_obj_set_hidden( obj, true );
    }
    else if( event == LV_EVENT_APPLY )
    {
        const char* txt = lv_ta_get_text( ta );
        int         len = strlen( txt );

        if( len == 0 )
        {
            lv_ta_set_text( ta, self->ta_text );
        }

        if( ta == self->ta_longitude )
        {
            self->settings_temp.longitude = atof( txt );
        }

        else if( ta == self->ta_latitude )
        {
            self->settings_temp.latitude = atof( txt );
        }

        self->ConfigActionButton( );

        lv_kb_set_ta( obj, NULL ); /*De-assign the text area*/
        lv_obj_set_hidden( obj, true );
    }
}
