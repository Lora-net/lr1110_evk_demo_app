/**
 * @file      guiTemperature.h
 *
 * @brief     Definition of the gui temperature page.
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

#include "guiTemperature.h"

#define GUI_TEMPERATURE_STR_BUFFER_LENGTH 30

GuiTemperature::GuiTemperature( const GuiTemperatureResult_t* results )
    : GuiCommon( GUI_PAGE_TEMPERATURE_DEMO ), is_running( false ), results( results )
{
    this->createHeaderWithIcons( "TEMPERATURE DEMO" );
    this->lbl_btn_start_stop = this->createActionButton( &( this->btn_start_stop ), "START", GuiTemperature::callback,
                                                         GUI_BUTTON_POS_CENTER, -60, true );
    this->createActionButton( &( this->btn_back ), "BACK", GuiTemperature::callback, GUI_BUTTON_POS_CENTER, -5, true );
    this->createInfoFrame( &( this->info_frame ), &( this->lbl_info_frame_1 ), "PRESS START TO BEGIN",
                           &( this->lbl_info_frame_2 ), "", &( this->lbl_info_frame_3 ), "" );
    lv_scr_load( this->screen );
}

GuiTemperature::~GuiTemperature( ) {}

void GuiTemperature::start( )
{
    printf( "Temperature gui start\n" );
    lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_ongoing ) );

    lv_label_set_text( this->lbl_info_frame_1, "Sending temperature" );
    lv_label_set_text( this->lbl_info_frame_2, "See Application Server" );
    lv_label_set_text( this->lbl_info_frame_3, "" );

    lv_label_set_text( this->lbl_btn_start_stop, "STOP" );
    this->is_running = true;
}

void GuiTemperature::stop( )
{
    lv_label_set_text( this->lbl_btn_start_stop, "START" );
    lv_label_set_text( this->lbl_info_frame_1, "PRESS START TO BEGIN" );
    lv_label_set_text( this->lbl_info_frame_2, "" );
    lv_label_set_text( this->lbl_info_frame_3, "" );
    lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_init ) );
    this->is_running = false;
}

void GuiTemperature::refresh( )
{
    char buffer[GUI_TEMPERATURE_STR_BUFFER_LENGTH] = { };
    snprintf( buffer, GUI_TEMPERATURE_STR_BUFFER_LENGTH, "Temperature: %4.2f C", this->results->temperature );
    lv_label_set_text( this->lbl_info_frame_1, buffer );

    if( this->results->sent == true )
    {
        lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_ok ) );
        this->is_running = false;
        lv_label_set_text( this->lbl_info_frame_2, "See Application Server" );
        lv_label_set_text( this->lbl_btn_start_stop, "START" );
    }
    else
    {
        lv_label_set_text( this->lbl_info_frame_2, "Sending..." );
        lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_ongoing ) );
    }

    this->updateButtons( );
}

void GuiTemperature::propagateHostConnectivityStateChange( ) { this->updateButtons( ); }

void GuiTemperature::propagateNetworkConnectivityStateChange( ) { this->updateButtons( ); }

void GuiTemperature::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiTemperature* self = ( GuiTemperature* ) lv_obj_get_user_data( obj );

    if( ( event == LV_EVENT_RELEASED ) && ( lv_btn_get_state( obj ) != LV_BTN_STATE_INA ) )
    {
        if( obj == self->btn_back )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
        else if( obj == self->btn_start_stop )
        {
            if( self->is_running == true )
            {
                GuiCommon::_event = GUI_EVENT_STOP;
            }
            else
            {
                GuiCommon::_event = GUI_EVENT_START_DEMO_TEMPERATURE;
            }
        }
    }
}

void GuiTemperature::updateButtons( )
{
    // if( ( ( GuiCommon::_network_connectivity_status.connectivity_state == GUI_CONNECTIVITY_STATUS_CONNECTED ) ||
    //       ( GuiCommon::_is_host_connected == true ) ) )
    // {
    //     lv_btn_set_state( this->btn_send, LV_BTN_STATE_REL );
    // }
    // else
    // {
    //     lv_btn_set_state( this->btn_send, LV_BTN_STATE_INA );
    // }
}
