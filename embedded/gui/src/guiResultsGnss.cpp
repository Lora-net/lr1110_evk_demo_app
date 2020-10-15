/**
 * @file      guiResultGnss.cpp
 *
 * @brief     Implementation of the gui result GNSS page.
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

#include "guiResultsGnss.h"

#define TMP_BUFFER_SET_RESULTS_LENGTH ( 25 )

GuiResultsGnss::GuiResultsGnss( const GuiGnssResult_t* results, guiPageType_t pageType )
    : GuiCommon( pageType ), _results( results )
{
    switch( pageType )
    {
    case GUI_PAGE_GNSS_AUTONOMOUS_RESULTS:
        this->createHeader( "GNSS AUTONOMOUS - RESULTS" );
        break;
    case GUI_PAGE_GNSS_ASSISTED_RESULTS:
        this->createHeader( "GNSS ASSISTED - RESULTS" );
        break;
    default:
        break;
    }

    this->createNetworkConnectivityIcon( &( this->_label_connectivity_icon ) );

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

    this->_pageCurrent = 0;

    this->_pageTotal = ( _results->nb_satellites % GUI_GNSS_MAX_RESULTS == 0 )
                           ? _results->nb_satellites / GUI_GNSS_MAX_RESULTS
                           : _results->nb_satellites / GUI_GNSS_MAX_RESULTS + 1;

    this->createActionButton( &( this->btn_back ), "BACK", GuiResultsGnss::callback, GUI_BUTTON_POS_CENTER, -5, true );

    this->createActionButton( &( this->btn_left ), "<", GuiResultsGnss::callback, GUI_BUTTON_POS_LEFT, -5,
                              ( this->_pageTotal > 1 ) ? true : false );

    this->createActionButton( &( this->btn_right ), ">", GuiResultsGnss::callback, GUI_BUTTON_POS_RIGHT, -5,
                              ( this->_pageTotal > 1 ) ? true : false );

    this->setResults( );

    lv_scr_load( this->screen );
}

GuiResultsGnss::~GuiResultsGnss( ) {}

void GuiResultsGnss::findAndDisplayNextPage( bool up )
{
    if( up == false )
    {
        this->_pageCurrent = ( this->_pageCurrent == 0 ) ? ( this->_pageTotal - 1 ) : ( this->_pageCurrent - 1 );
    }
    else
    {
        this->_pageCurrent = ( this->_pageCurrent + 1 ) % this->_pageTotal;
    }

    this->setResults( );
}

void GuiResultsGnss::setResults( )
{
    char    buffer[TMP_BUFFER_SET_RESULTS_LENGTH];
    uint8_t max;

    snprintf( buffer, TMP_BUFFER_SET_RESULTS_LENGTH, "Page %i/%i", _pageCurrent + 1, _pageTotal );
    lv_label_set_text( this->lbl_info_page, buffer );

    max = ( ( ( _pageCurrent + 1 ) * GUI_GNSS_MAX_RESULTS ) > _results->nb_satellites )
              ? _results->nb_satellites
              : ( ( _pageCurrent + 1 ) * GUI_GNSS_MAX_RESULTS );

    lv_table_set_row_cnt( this->table, max - _pageCurrent * GUI_GNSS_MAX_RESULTS );

    for( uint8_t index = _pageCurrent * GUI_GNSS_MAX_RESULTS; index < max; index++ )
    {
        uint8_t table_index = index - _pageCurrent * GUI_GNSS_MAX_RESULTS;

        switch( _results->satellite_ids[index].constellation )
        {
        case GUI_GNSS_CONSTELLATION_GPS:
            snprintf( buffer, TMP_BUFFER_SET_RESULTS_LENGTH, "GPS #%i", _results->satellite_ids[index].satellite_id );
            break;
        case GUI_GNSS_CONSTELLATION_BEIDOU:
            snprintf( buffer, TMP_BUFFER_SET_RESULTS_LENGTH, "BeiDou #%i",
                      _results->satellite_ids[index].satellite_id );
            break;
        default:
            break;
        }

        lv_table_set_cell_value( this->table, table_index, 0, buffer );
        lv_table_set_cell_align( this->table, table_index, 0, LV_LABEL_ALIGN_LEFT );

        snprintf( buffer, TMP_BUFFER_SET_RESULTS_LENGTH, "%ddB", _results->satellite_ids[index].snr );
        lv_table_set_cell_value( this->table, table_index, 1, buffer );
        lv_table_set_cell_align( this->table, table_index, 1, LV_LABEL_ALIGN_RIGHT );
    }
}

void GuiResultsGnss::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiResultsGnss* self = ( GuiResultsGnss* ) lv_obj_get_user_data( obj );

    if( ( event == LV_EVENT_RELEASED ) && ( lv_btn_get_state( obj ) != LV_BTN_STATE_INA ) )
    {
        if( obj == self->btn_back )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
        else if( obj == self->btn_left )
        {
            self->findAndDisplayNextPage( false );
        }
        else if( obj == self->btn_right )
        {
            self->findAndDisplayNextPage( true );
        }
    }
}
