/**
 * @file      guiConnectivity.cpp
 *
 * @brief     Implementation of the GUI connectivity page
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

#include "guiConnectivity.h"

GuiConnectivity::GuiConnectivity( GuiNetworkConnectivitySettings_t* network_connectivity_settings )
    : GuiCommon( GUI_PAGE_CONNECTIVITY ), _network_connectivity_settings( network_connectivity_settings )
{
    this->createHeaderWithIcons( "CONNECTIVITY" );

    this->lbl_connectivity_state = lv_label_create( this->screen, NULL );
    lv_label_set_long_mode( this->lbl_connectivity_state, LV_LABEL_LONG_BREAK );
    lv_label_set_recolor( this->lbl_connectivity_state, true );
    lv_label_set_align( this->lbl_connectivity_state, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( this->lbl_connectivity_state, "State: Not connected" );
    lv_obj_set_width( this->lbl_connectivity_state, 150 );
    lv_obj_align( this->lbl_connectivity_state, NULL, LV_ALIGN_CENTER, 0, 80 );

    this->btn_join = lv_btn_create( this->screen, NULL );
    lv_btn_set_state( this->btn_join, LV_BTN_STATE_REL );
    lv_obj_set_height( this->btn_join, 40 );
    lv_obj_set_width( this->btn_join, 150 );
    lv_obj_set_event_cb( this->btn_join, GuiConnectivity::callback );
    lv_obj_set_user_data( this->btn_join, this );
    lv_obj_align( this->btn_join, NULL, LV_ALIGN_CENTER, 0, 40 );
    this->lbl_btn_join = lv_label_create( this->btn_join, NULL );
    lv_label_set_text( this->lbl_btn_join, "JOIN" );

    // Make sure the string is defined to follow the order specified in GuiNetworkConnectivityLorawanRegion_t
    this->createDropDownList( &( this->ddlist_lorawan_region ), this->screen, 60, "LoRaWAN region",
                              "EU868\n"
                              "AS923_G1\n"
                              "US915\n"
                              "AU915\n"
                              "CN470\n"
                              "AS923_G2\n"
                              "AS923_G3\n"
                              "IN865\n"
                              "KR920\n"
                              "RU864",
                              GuiConnectivity::callback_ddlist, 100,
                              ( uint16_t ) this->_network_connectivity_settings->region );

    // Make sure the string is defined to follow the order specified in GuiNetworkConnectivityAdrProfile_t
    this->createDropDownList( &( this->ddlist_lorawan_class ), this->screen, 100, "LoRaWAN class",
                              "Class A\n"
                              "Class C",
                              GuiConnectivity::callback_ddlist, 100,
                              ( uint16_t ) this->_network_connectivity_settings->lorawan_class );

    // Make sure the string is defined to follow the order specified in GuiNetworkConnectivityAdrProfile_t
    this->createDropDownList( &( this->ddlist_adr_profile ), this->screen, 140, "ADR profile",
                              "NS controlled\n"
                              "Mobile LR\n"
                              "Mobile LP",
                              GuiConnectivity::callback_ddlist, 130,
                              ( uint16_t ) this->_network_connectivity_settings->adr_profile );

    this->createActionButton( &( this->btn_back ), "BACK", GuiConnectivity::callback, GUI_BUTTON_POS_CENTER, -5, true );

    this->updateButtons( );

    lv_scr_load( this->screen );
}

GuiConnectivity::~GuiConnectivity( ) {}

void GuiConnectivity::propagateNetworkConnectivityStateChange( ) { this->updateButtons( ); }

void GuiConnectivity::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiConnectivity* self = ( GuiConnectivity* ) lv_obj_get_user_data( obj );

    if( event == LV_EVENT_RELEASED )
    {
        if( obj == self->btn_join )
        {
            char                           text[10];
            GuiNetworkConnectivityStatus_t guiNetworkConnectivityStatus;
            strcpy( text, lv_label_get_text( self->lbl_btn_join ) );

            if( memcmp( text, "JOIN", 4 ) == 0 )
            {
                guiNetworkConnectivityStatus.connectivity_state = GUI_CONNECTIVITY_STATUS_JOINING;
                GuiCommon::_event                               = GUI_EVENT_JOIN;
            }
            else if( memcmp( text, "ABORT", 4 ) == 0 )
            {
                guiNetworkConnectivityStatus.connectivity_state = GUI_CONNECTIVITY_STATUS_NOT_CONNECTED;
                GuiCommon::_event                               = GUI_EVENT_ABORT;
            }
            else if( memcmp( text, "LEAVE", 4 ) == 0 )
            {
                guiNetworkConnectivityStatus.connectivity_state = GUI_CONNECTIVITY_STATUS_NOT_CONNECTED;
                GuiCommon::_event                               = GUI_EVENT_LEAVE;
            }

            guiNetworkConnectivityStatus.is_time_sync = GuiCommon::_network_connectivity_status.is_time_sync;
            self->updateNetworkConnectivityState( &guiNetworkConnectivityStatus );
        }
        else if( obj == self->btn_back )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
    }
}

void GuiConnectivity::callback_ddlist( lv_obj_t* obj, lv_event_t event )
{
    uint8_t          id;
    GuiConnectivity* self = ( GuiConnectivity* ) lv_obj_get_user_data( obj );

    if( event == LV_EVENT_VALUE_CHANGED )
    {
        id = lv_ddlist_get_selected( obj );

        if( obj == self->ddlist_lorawan_region )
        {
            self->_network_connectivity_settings->region = ( GuiNetworkConnectivityLorawanRegion_t ) id;
        }
        else if( obj == self->ddlist_lorawan_class )
        {
            self->_network_connectivity_settings->lorawan_class = ( GuiNetworkConnectivityLorawanClass_t ) id;
        }
        else if( obj == self->ddlist_adr_profile )
        {
            self->_network_connectivity_settings->adr_profile = ( GuiNetworkConnectivityAdrProfile_t ) id;
        }
    }
}

void GuiConnectivity::updateButtons( )
{
    switch( GuiCommon::_network_connectivity_status.connectivity_state )
    {
    case GUI_CONNECTIVITY_STATUS_NOT_CONNECTED:
    {
        lv_label_set_text( this->lbl_btn_join, "JOIN" );
        lv_label_set_text( this->lbl_connectivity_state, "State: Not connected" );
        lv_obj_set_click( this->ddlist_lorawan_region, true );
        lv_obj_set_click( this->ddlist_lorawan_class, true );
        break;
    }
    case GUI_CONNECTIVITY_STATUS_JOINING:
    {
        lv_label_set_text( this->lbl_btn_join, "ABORT" );
        lv_label_set_text( this->lbl_connectivity_state, "State: Joining..." );
        lv_obj_set_click( this->ddlist_lorawan_region, false );
        lv_obj_set_click( this->ddlist_lorawan_class, false );
        break;
    }
    case GUI_CONNECTIVITY_STATUS_CONNECTED:
    {
        lv_label_set_text( this->lbl_btn_join, "LEAVE" );
        lv_label_set_text( this->lbl_connectivity_state, "State: Connected" );
        lv_obj_set_click( this->ddlist_lorawan_region, false );
        lv_obj_set_click( this->ddlist_lorawan_class, false );
        break;
    }
    default:
        break;
    }
}
