/**
 * @file      guiMenuGeolocDemo.cpp
 *
 * @brief     Implementation of the gui GeoLoc demo menu page.
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

#include "guiMenuGeolocDemo.h"

#define MARGIN 10

GuiMenuGeolocDemo::GuiMenuGeolocDemo( version_handler_t* version_handler ) : GuiMenuCommon( GUI_PAGE_MENU_GEOLOC_DEMO )
{
    uint8_t index = 0;

    this->createHeaderWithIcons( "GEOLOCATION" );

    this->createTestEntry( index++, &( this->lbl_wifi ), &( this->btn_wifi ), &( this->lbl_btn_wifi ), "Wi-Fi scan",
                           true, GuiMenuGeolocDemo::callback );

    this->createTestEntry(
        index++, &( this->lbl_gnss_autonomous ), &( this->btn_gnss_autonomous ), &( this->lbl_btn_gnss_autonomous ),
        ( GuiCommon::_is_host_connected == true ) || ( GuiCommon::_network_connectivity_status.is_time_sync == true )
            ? "GNSS autonomous"
            : "GNSS autonomous*",
        ( GuiCommon::_is_host_connected == true ) || ( GuiCommon::_network_connectivity_status.is_time_sync == true )
            ? true
            : false,
        GuiMenuGeolocDemo::callback );

    this->createTestEntry(
        index++, &( this->lbl_gnss_assisted ), &( this->btn_gnss_assisted ), &( this->lbl_btn_gnss_assisted ),
        ( GuiCommon::_is_host_connected == true ) || ( GuiCommon::_network_connectivity_status.is_time_sync == true )
            ? "GNSS assisted"
            : "GNSS assisted*",
        ( GuiCommon::_is_host_connected == true ) || ( GuiCommon::_network_connectivity_status.is_time_sync == true )
            ? true
            : false,
        GuiMenuGeolocDemo::callback );

    this->createActionButton( &( this->btn_back ), "BACK", GuiMenuGeolocDemo::callback, GUI_BUTTON_POS_CENTER, -5,
                              true );

    this->lbl_connectivity = lv_label_create( this->screen, NULL );
    lv_obj_set_style( this->lbl_connectivity, &( GuiCommon::note_style ) );
    lv_label_set_long_mode( this->lbl_connectivity, LV_LABEL_LONG_BREAK );
    lv_label_set_align( this->lbl_connectivity, LV_LABEL_ALIGN_LEFT );

    switch( version_handler->device_type )
    {
    case VERSION_DEVICE_TRANSCEIVER:
    {
        lv_label_set_text( this->lbl_connectivity, "* Launch host app" );
        break;
    }
    case VERSION_DEVICE_MODEM:
    {
        lv_label_set_text( this->lbl_connectivity, "* Wait for time sync" );
        break;
    }
    default:
    {
        lv_label_set_text( this->lbl_connectivity, "* Error" );
        break;
    }
    }

    lv_obj_set_width( this->lbl_connectivity, 80 );
    lv_obj_align( this->lbl_connectivity, NULL, LV_ALIGN_IN_BOTTOM_LEFT, MARGIN, -10 );
    lv_obj_set_hidden( this->lbl_connectivity, ( ( GuiCommon::_is_host_connected == true ) ||
                                                 ( GuiCommon::_network_connectivity_status.is_time_sync == true ) )
                                                   ? true
                                                   : false );

    lv_scr_load( this->screen );
}

GuiMenuGeolocDemo::~GuiMenuGeolocDemo( ) {}

void GuiMenuGeolocDemo::propagateHostConnectivityStateChange( ) { this->updateButtons( ); }

void GuiMenuGeolocDemo::propagateNetworkConnectivityStateChange( ) { this->updateButtons( ); }

void GuiMenuGeolocDemo::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiMenuGeolocDemo* self = ( GuiMenuGeolocDemo* ) lv_obj_get_user_data( obj );

    if( ( event == LV_EVENT_RELEASED ) && ( lv_btn_get_state( obj ) != LV_BTN_STATE_INA ) )
    {
        if( obj == self->btn_wifi )
        {
            GuiCommon::_event = GUI_EVENT_START_WIFI;
        }
        else if( obj == self->btn_gnss_autonomous )
        {
            GuiCommon::_event = GUI_EVENT_START_GNSS_AUTONOMOUS;
        }
        else if( obj == self->btn_gnss_assisted )
        {
            GuiCommon::_event = GUI_EVENT_START_GNSS_ASSISTED;
        }
        else if( obj == self->btn_back )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
    }
}

void GuiMenuGeolocDemo::updateButtons( )
{
    lv_btn_set_state( btn_gnss_autonomous, ( ( GuiCommon::_is_host_connected == true ) ||
                                             ( GuiCommon::_network_connectivity_status.is_time_sync == true ) )
                                               ? LV_BTN_STATE_REL
                                               : LV_BTN_STATE_INA );

    lv_btn_set_state( btn_gnss_assisted, ( ( GuiCommon::_is_host_connected == true ) ||
                                           ( GuiCommon::_network_connectivity_status.is_time_sync == true ) )
                                             ? LV_BTN_STATE_REL
                                             : LV_BTN_STATE_INA );

    if( ( GuiCommon::_is_host_connected == true ) || ( GuiCommon::_network_connectivity_status.is_time_sync == true ) )
    {
        lv_obj_set_hidden( this->lbl_connectivity, true );
        lv_label_set_text( this->lbl_gnss_autonomous, "GNSS autonomous" );
        lv_label_set_text( this->lbl_gnss_assisted, "GNSS assisted" );
    }
    else
    {
        lv_obj_set_hidden( this->lbl_connectivity, false );
        lv_label_set_text( this->lbl_gnss_autonomous, "GNSS autonomous*" );
        lv_label_set_text( this->lbl_gnss_assisted, "GNSS assisted*" );
    }
}
