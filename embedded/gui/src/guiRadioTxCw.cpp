/**
 * @file      guiRadioTxCw.cpp
 *
 * @brief     Implementation of the gui Continuous Wave Transmit demo page.
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

#include "guiRadioTxCw.h"

#define TMP_BUFFER_REFRESH_LENGTH ( 30 )

GuiRadioTxCw::GuiRadioTxCw( const GuiRadioSetting_t* settings )
    : GuiCommon( GUI_PAGE_RADIO_TX_CW ), settings( settings )
{
    char buffer_1[TMP_BUFFER_REFRESH_LENGTH] = { 0 };
    char buffer_2[TMP_BUFFER_REFRESH_LENGTH] = { 0 };
    char buffer_3[TMP_BUFFER_REFRESH_LENGTH] = { 0 };

    this->createHeaderWithIcons( "TX CONTINUOUS WAVE" );

    snprintf( buffer_1, TMP_BUFFER_REFRESH_LENGTH, "Freq = %i Hz", this->settings->rf_freq_in_hz );
    snprintf( buffer_2, TMP_BUFFER_REFRESH_LENGTH, "Power = %i dBm", this->settings->pwr_in_dbm );
    snprintf( buffer_3, TMP_BUFFER_REFRESH_LENGTH, "PRESS START TO BEGIN" );
    this->createInfoFrame( &( this->info_frame ), &( this->lbl_info_frame_1 ), buffer_1, &( this->lbl_info_frame_2 ),
                           buffer_2, &( this->lbl_info_frame_3 ), buffer_3 );

    this->createActionButton( &( this->btn_start ), "START", GuiRadioTxCw::callback, GUI_BUTTON_POS_CENTER, -60, true );

    this->createActionButton( &( this->btn_stop ), "STOP", GuiRadioTxCw::callback, GUI_BUTTON_POS_CENTER, -60, true );

    this->createActionButton( &( this->btn_back ), "BACK", GuiRadioTxCw::callback, GUI_BUTTON_POS_CENTER, -5, true );

    lv_obj_set_hidden( this->btn_start, false );
    lv_obj_set_hidden( this->btn_stop, true );

    lv_scr_load( this->screen );
}

GuiRadioTxCw::~GuiRadioTxCw( ) {}

void GuiRadioTxCw::start( )
{
    lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_ongoing ) );

    lv_label_set_text( this->lbl_info_frame_3, "TRANSMITTING..." );
}

void GuiRadioTxCw::stop( )
{
    lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_init ) );

    lv_label_set_text( this->lbl_info_frame_3, "PRESS START TO BEGIN" );
}

void GuiRadioTxCw::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiRadioTxCw* self = ( GuiRadioTxCw* ) lv_obj_get_user_data( obj );

    if( ( event == LV_EVENT_RELEASED ) && ( lv_btn_get_state( obj ) != LV_BTN_STATE_INA ) )
    {
        if( obj == self->btn_start )
        {
            lv_obj_set_hidden( self->btn_start, true );
            lv_obj_set_hidden( self->btn_stop, false );
            GuiCommon::_event = GUI_EVENT_START_TX_CW;
        }
        if( obj == self->btn_stop )
        {
            lv_obj_set_hidden( self->btn_start, false );
            lv_obj_set_hidden( self->btn_stop, true );
            GuiCommon::_event = GUI_EVENT_STOP;
        }
        else if( obj == self->btn_back )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
    }
}
