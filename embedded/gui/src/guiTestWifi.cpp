/**
 * @file      guiTestWifi.cpp
 *
 * @brief     Implementation of the gui test Wi-Fi page.
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

#include "guiTestWifi.h"

#define TMP_BUFFERS_REFRESH_LENGTH ( 30 )
#define TMP_BUFFER_REVERSE_GEO_LOC_REFRESH_LENGTH ( 80 )

GuiTestWifi::GuiTestWifi( const GuiWifiResult_t* results, bool at_least_one_scan_done )
    : GuiCommon( GUI_PAGE_WIFI_TEST ), _results( results )
{
    this->createHeader( "Wi-Fi SCANNING" );

    this->createNetworkConnectivityIcon( &( this->_label_connectivity_icon ) );

    this->createActionButton( &( this->btn_start_stop ), "START", GuiTestWifi::callback, GUI_BUTTON_POS_CENTER, -60,
                              true );

    this->createActionButton( &( this->btn_results ), "RESULTS", GuiTestWifi::callback, GUI_BUTTON_POS_LEFT, -60,
                              false );

    this->createActionButton( &( this->btn_send ), "SEND", GuiTestWifi::callback, GUI_BUTTON_POS_RIGHT, -60, false );

    this->createActionButton( &( this->btn_back ), "BACK", GuiTestWifi::callback, GUI_BUTTON_POS_CENTER, -5, true );

    this->createActionButton( &( this->btn_config ), "CONFIG", GuiTestWifi::callback, GUI_BUTTON_POS_RIGHT, -5, true );

    this->createInfoFrame( &( this->info_frame ), &( this->lbl_info_frame_1 ), "PRESS START TO BEGIN",
                           &( this->lbl_info_frame_2 ), "", &( this->lbl_info_frame_3 ), "" );

    if( at_least_one_scan_done == true )
    {
        this->refresh( );
    }

    lv_scr_load( this->screen );
}

GuiTestWifi::~GuiTestWifi( ) {}

void GuiTestWifi::start( )
{
    lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_ongoing ) );

    lv_label_set_text( this->lbl_info_frame_1, "SCAN IN PROGRESS..." );
    lv_label_set_text( this->lbl_info_frame_2, "" );
    lv_label_set_text( this->lbl_info_frame_3, "" );

    lv_btn_set_state( this->btn_send, LV_BTN_STATE_INA );
    lv_btn_set_state( this->btn_results, LV_BTN_STATE_INA );
}

void GuiTestWifi::refresh( )
{
    char buffer_1[TMP_BUFFERS_REFRESH_LENGTH]                = { 0 };
    char buffer_2[TMP_BUFFERS_REFRESH_LENGTH]                = { 0 };
    char buffer_3[TMP_BUFFER_REVERSE_GEO_LOC_REFRESH_LENGTH] = { 0 };

    if( _results->error == true )
    {
        lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_error ) );

        snprintf( buffer_1, TMP_BUFFERS_REFRESH_LENGTH, "Error" );
    }
    else
    {
        lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_ok ) );

        if( _results->nbMacAddrTotal > 1 )
        {
            snprintf( buffer_1, TMP_BUFFERS_REFRESH_LENGTH, "%i MAC addresses found", _results->nbMacAddrTotal );
        }
        else
        {
            snprintf( buffer_1, TMP_BUFFERS_REFRESH_LENGTH, "%i MAC address found", _results->nbMacAddrTotal );
        }

        snprintf( buffer_2, TMP_BUFFERS_REFRESH_LENGTH, "%u ms / %.3f uAh", _results->timingTotal,
                  GuiCommon::convertConsoToUah( _results->powerConsTotal ) );

        if( _results->nbMacAddrTotal != 0 )
        {
            lv_btn_set_state( this->btn_results, LV_BTN_STATE_REL );
            if( GuiCommon::_is_host_connected == true )
            {
                lv_btn_set_state( this->btn_send, LV_BTN_STATE_REL );
            }
        }
    }

    if( strlen( _results->reverse_geo_loc.latitude ) != 0 )
    {
        if( strlen( _results->reverse_geo_loc.country ) != 0 )
        {
            snprintf( buffer_3, TMP_BUFFER_REVERSE_GEO_LOC_REFRESH_LENGTH, "%s %s\n%s - %s\n%s",
                      _results->reverse_geo_loc.latitude, _results->reverse_geo_loc.longitude,
                      _results->reverse_geo_loc.country, _results->reverse_geo_loc.city,
                      _results->reverse_geo_loc.street );
        }
        else
        {
            snprintf( buffer_3, TMP_BUFFER_REVERSE_GEO_LOC_REFRESH_LENGTH, "Lat.: %s\nLon.: %s",
                      _results->reverse_geo_loc.latitude, _results->reverse_geo_loc.longitude );
        }
    }

    lv_label_set_text( this->lbl_info_frame_1, buffer_1 );
    lv_label_set_text( this->lbl_info_frame_2, buffer_2 );
    lv_label_set_text( this->lbl_info_frame_3, buffer_3 );

    this->updateButtons( );
}

void GuiTestWifi::updateHostConnectivityState( void ) { this->updateButtons( ); }

void GuiTestWifi::updateNetworkConnectivityState( ) { this->updateButtons( ); }

void GuiTestWifi::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiTestWifi* self = ( GuiTestWifi* ) lv_obj_get_user_data( obj );

    if( ( event == LV_EVENT_RELEASED ) && ( lv_btn_get_state( obj ) != LV_BTN_STATE_INA ) )
    {
        if( obj == self->btn_back )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
        else if( obj == self->btn_start_stop )
        {
            GuiCommon::_event = GUI_EVENT_START_WIFI;
        }
        else if( obj == self->btn_results )
        {
            GuiCommon::_event = GUI_EVENT_RESULTS;
        }
        else if( obj == self->btn_send )
        {
            GuiCommon::_event = GUI_EVENT_SEND;
        }
        else if( obj == self->btn_config )
        {
            GuiCommon::_event = GUI_EVENT_CONFIG;
        }
    }
}

void GuiTestWifi::updateButtons( )
{
    if( ( ( GuiCommon::_is_host_connected == true ) ||
          ( GuiCommon::_network_connectivity_status.connectivity_state == GUI_CONNECTIVITY_STATUS_CONNECTED ) ) &&
        ( _results->nbMacAddrTotal > 0 ) )
    {
        lv_btn_set_state( this->btn_send, LV_BTN_STATE_REL );
    }
    else
    {
        lv_btn_set_state( this->btn_send, LV_BTN_STATE_INA );
    }
}
