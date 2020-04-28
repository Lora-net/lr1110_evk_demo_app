/**
 * @file      guiResultWifi.cpp
 *
 * @brief     Implementation of the gui result Wi-Fi page.
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

#include "guiResultsWifi.h"

#define TMP_BUFFER_SET_CHANNEL_RESULTS_LENGTH ( 25 )

GuiResultsWifi::GuiResultsWifi( const GuiWifiResult_t* results ) : GuiCommon( ), _results( results )
{
    _pageType = GUI_PAGE_WIFI_RESULTS;

    this->screen = lv_obj_create( NULL, NULL );
    lv_obj_set_style( this->screen, &( GuiCommon::screen_style ) );

    this->createHeader( "Wi-Fi SCANNING - RESULTS" );

    this->createActionButton( &( this->btn_back ), "BACK", GuiResultsWifi::callback, GUI_BUTTON_POS_CENTER, -5, true );

    this->createActionButton( &( this->btn_left ), "<", GuiResultsWifi::callback, GUI_BUTTON_POS_LEFT, -5, false );

    this->createActionButton( &( this->btn_right ), ">", GuiResultsWifi::callback, GUI_BUTTON_POS_RIGHT, -5, false );

    this->lbl_info_page = lv_label_create( this->screen, NULL );
    lv_obj_set_style( this->lbl_info_page, &( GuiCommon::screen_style ) );
    lv_label_set_long_mode( this->lbl_info_page, LV_LABEL_LONG_BREAK );
    lv_label_set_align( this->lbl_info_page, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( this->lbl_info_page, "" );
    lv_obj_set_width( this->lbl_info_page, 240 );
    lv_obj_align( this->lbl_info_page, NULL, LV_ALIGN_IN_TOP_MID, 0, 50 );

    this->table = lv_table_create( this->screen, NULL );
    lv_table_set_style( this->table, LV_TABLE_STYLE_CELL1, &( GuiCommon::table_cell1 ) );
    lv_table_set_style( this->table, LV_TABLE_STYLE_BG, &lv_style_transp_tight );
    lv_table_set_col_cnt( this->table, 2 );
    lv_table_set_row_cnt( this->table, 6 );
    lv_table_set_col_width( this->table, 0, 150 );
    lv_table_set_col_width( this->table, 1, 80 );
    lv_obj_align( this->table, NULL, LV_ALIGN_IN_TOP_MID, 0, 80 );
}

GuiResultsWifi::~GuiResultsWifi( ) { lv_obj_del( this->screen ); }

void GuiResultsWifi::draw( )
{
    const GuiWifiResultChannel_t* chan = NULL;

    // Find the first non - empty channel
    for( uint8_t index = 0; index < GUI_WIFI_CHANNELS * 2; index++ )
    {
        if( index < GUI_WIFI_CHANNELS )
        {
            if( _results->typeB.channel[index].nbMacAddr > 0 )
            {
                chan   = &_results->typeB.channel[index];
                _index = index;
                break;
            }
        }
        else
        {
            if( _results->typeG.channel[index % GUI_WIFI_CHANNELS].nbMacAddr > 0 )
            {
                chan   = &_results->typeG.channel[index % GUI_WIFI_CHANNELS];
                _index = index;
                break;
            }
        }
    }

    // Update navigation button states
    if( ( chan != NULL ) && ( chan->nbMacAddr != _results->nbMacAddrTotal ) )
    {
        lv_btn_set_state( this->btn_left, LV_BTN_STATE_REL );
        lv_btn_set_state( this->btn_right, LV_BTN_STATE_REL );
    }
    else
    {
        lv_btn_set_state( this->btn_left, LV_BTN_STATE_INA );
        lv_btn_set_state( this->btn_right, LV_BTN_STATE_INA );
    }

    this->setChannelResults( );

    lv_scr_load( this->screen );
}

void GuiResultsWifi::updateResults( guiEvent_t event )
{
    this->findNextChannel( ( event == GUI_EVENT_LEFT ) ? false : true );
    this->setChannelResults( );
}

void GuiResultsWifi::setChannelResults( )
{
    char                          buffer[TMP_BUFFER_SET_CHANNEL_RESULTS_LENGTH];
    const GuiWifiResultChannel_t* chan;

    // Create page information
    if( this->_index < GUI_WIFI_CHANNELS )
    {
        chan = &_results->typeB.channel[this->_index];
        snprintf( buffer, TMP_BUFFER_SET_CHANNEL_RESULTS_LENGTH, "Wi-Fi B Ch%i", ( this->_index + 1 ) );
    }
    else
    {
        chan = &_results->typeG.channel[this->_index % GUI_WIFI_CHANNELS];
        snprintf( buffer, TMP_BUFFER_SET_CHANNEL_RESULTS_LENGTH, "Wi-Fi G Ch%i", ( _index + 1 ) % GUI_WIFI_CHANNELS );
    }

    lv_label_set_text( this->lbl_info_page, buffer );

    lv_table_set_row_cnt( this->table, chan->nbMacAddr );

    for( uint8_t index = 0; index < chan->nbMacAddr; index++ )
    {
        lv_table_set_cell_value( table, index, 0, chan->data[index].macAddr );
        lv_table_set_cell_align( table, index, 0, LV_LABEL_ALIGN_LEFT );

        snprintf( buffer, TMP_BUFFER_SET_CHANNEL_RESULTS_LENGTH, "%idBm", chan->data[index].rssi );
        lv_table_set_cell_value( table, index, 1, buffer );
        lv_table_set_cell_align( table, index, 1, LV_LABEL_ALIGN_RIGHT );
    }
}

void GuiResultsWifi::findNextChannel( bool up )
{
    while( true )
    {
        if( up == true )
        {
            _index = ( _index + 1 ) % ( GUI_WIFI_CHANNELS * 2 );
        }
        else
        {
            _index = ( _index == 0 ) ? ( GUI_WIFI_CHANNELS * 2 - 1 ) : ( _index - 1 );
        }

        if( _index < GUI_WIFI_CHANNELS )
        {
            if( _results->typeB.channel[_index].nbMacAddr > 0 )
            {
                break;
            }
        }
        else
        {
            if( _results->typeG.channel[_index % GUI_WIFI_CHANNELS].nbMacAddr > 0 )
            {
                break;
            }
        }
    }
}

void GuiResultsWifi::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiResultsWifi* self = ( GuiResultsWifi* ) lv_obj_get_user_data( obj );

    if( ( event == LV_EVENT_RELEASED ) && ( lv_btn_get_state( obj ) != LV_BTN_STATE_INA ) )
    {
        if( obj == self->btn_back )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
        else if( obj == self->btn_left )
        {
            GuiCommon::_event = GUI_EVENT_LEFT;
        }
        else if( obj == self->btn_right )
        {
            GuiCommon::_event = GUI_EVENT_RIGHT;
        }
    }
}
