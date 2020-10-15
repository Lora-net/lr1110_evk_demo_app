/**
 * @file      guiRadioPer.cpp
 *
 * @brief     Implementation of the gui Packet Error Rate page.
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

#include "guiRadioPer.h"

#define TMP_BUFFER_REFRESH_LENGTH ( 30 )

GuiRadioPer::GuiRadioPer( const GuiRadioSetting_t* settings, const GuiRadioPerResult_t* results )
    : GuiCommon( GUI_PAGE_RADIO_PER ), settings( settings ), results( results )
{
    this->createHeader( "PACKET ERROR RATE" );

    this->createNetworkConnectivityIcon( &( this->_label_connectivity_icon ) );

    this->createInfoFrame( &( this->info_frame ), &( this->lbl_info_frame_1 ), "CHOOSE TX OR RX",
                           &( this->lbl_info_frame_2 ), "", &( this->lbl_info_frame_3 ), "" );

    this->createActionButton( &( this->btn_start_tx ), "TX", GuiRadioPer::callback, GUI_BUTTON_POS_LEFT, -60, true );

    this->createActionButton( &( this->btn_start_rx ), "RX", GuiRadioPer::callback, GUI_BUTTON_POS_RIGHT, -60, true );

    this->createActionButton( &( this->btn_stop ), "STOP", GuiRadioPer::callback, GUI_BUTTON_POS_CENTER, -60, true );

    this->createActionButton( &( this->btn_back ), "BACK", GuiRadioPer::callback, GUI_BUTTON_POS_CENTER, -5, true );

    lv_obj_set_hidden( this->btn_start_tx, false );
    lv_obj_set_hidden( this->btn_start_rx, false );
    lv_obj_set_hidden( this->btn_stop, true );

    lv_scr_load( this->screen );
}

GuiRadioPer::~GuiRadioPer( ) {}

void GuiRadioPer::start( )
{
    lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_ongoing ) );

    if( this->is_tx == true )
    {
        lv_label_set_text( this->lbl_info_frame_1, "Packets sent = 0" );
        lv_label_set_text( this->lbl_info_frame_2, "" );
        lv_label_set_text( this->lbl_info_frame_3, "" );
    }
    else
    {
        lv_label_set_text( this->lbl_info_frame_1, "Packets received = 0" );
        lv_label_set_text( this->lbl_info_frame_2, "Packet errors = 0" );
        lv_label_set_text( this->lbl_info_frame_3, "" );
    }
}

void GuiRadioPer::stop( )
{
    lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_init ) );

    lv_label_set_text( this->lbl_info_frame_1, "CHOOSE TX OR RX" );
    lv_label_set_text( this->lbl_info_frame_2, "" );
    lv_label_set_text( this->lbl_info_frame_3, "" );
}

void GuiRadioPer::refresh( )
{
    char buffer[TMP_BUFFER_REFRESH_LENGTH] = { 0 };

    if( this->is_tx == true )
    {
        snprintf( buffer, TMP_BUFFER_REFRESH_LENGTH, "Packets sent = %i", this->results->count_tx );
        lv_label_set_text( this->lbl_info_frame_1, buffer );

        if( this->settings->nb_of_packets == this->results->count_tx )
        {
            lv_obj_set_hidden( this->btn_start_tx, false );
            lv_obj_set_hidden( this->btn_start_rx, false );
            lv_obj_set_hidden( this->btn_stop, true );
            GuiCommon::_event = GUI_EVENT_STOP;
        }
    }
    else
    {
        snprintf( buffer, TMP_BUFFER_REFRESH_LENGTH, "Packets received = %i", this->results->count_rx_correct_packet );
        lv_label_set_text( this->lbl_info_frame_1, buffer );

        snprintf( buffer, TMP_BUFFER_REFRESH_LENGTH, "Packet errors = %i", this->results->count_rx_wrong_packet );
        lv_label_set_text( this->lbl_info_frame_2, buffer );
    }
}

void GuiRadioPer::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiRadioPer* self = ( GuiRadioPer* ) lv_obj_get_user_data( obj );

    if( ( event == LV_EVENT_RELEASED ) && ( lv_btn_get_state( obj ) != LV_BTN_STATE_INA ) )
    {
        if( obj == self->btn_start_tx )
        {
            self->is_tx = true;
            lv_obj_set_hidden( self->btn_start_tx, true );
            lv_obj_set_hidden( self->btn_start_rx, true );
            lv_obj_set_hidden( self->btn_stop, false );
            GuiCommon::_event = GUI_EVENT_START_PER_TX;
        }
        else if( obj == self->btn_start_rx )
        {
            self->is_tx = false;
            lv_obj_set_hidden( self->btn_start_tx, true );
            lv_obj_set_hidden( self->btn_start_rx, true );
            lv_obj_set_hidden( self->btn_stop, false );
            GuiCommon::_event = GUI_EVENT_START_PER_RX;
        }
        else if( obj == self->btn_stop )
        {
            lv_obj_set_hidden( self->btn_start_tx, false );
            lv_obj_set_hidden( self->btn_start_rx, false );
            lv_obj_set_hidden( self->btn_stop, true );
            GuiCommon::_event = GUI_EVENT_STOP;
        }
        else if( obj == self->btn_back )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
    }
}
