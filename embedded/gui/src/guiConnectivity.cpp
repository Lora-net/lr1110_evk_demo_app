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
    this->createHeader( "CONNECTIVITY" );

    this->createNetworkConnectivityIcon( &( this->_label_connectivity_icon ) );

    this->lbl_connectivity_state = lv_label_create( this->screen, NULL );
    lv_label_set_long_mode( this->lbl_connectivity_state, LV_LABEL_LONG_BREAK );
    lv_label_set_recolor( this->lbl_connectivity_state, true );
    lv_label_set_align( this->lbl_connectivity_state, LV_LABEL_ALIGN_CENTER );
    lv_label_set_text( this->lbl_connectivity_state, "State: Not connected" );
    lv_obj_set_width( this->lbl_connectivity_state, 150 );
    lv_obj_align( this->lbl_connectivity_state, NULL, LV_ALIGN_CENTER, 0, -100 );

    this->btn_join = lv_btn_create( this->screen, NULL );
    lv_btn_set_state( this->btn_join, LV_BTN_STATE_REL );
    lv_obj_set_height( this->btn_join, 40 );
    lv_obj_set_width( this->btn_join, 150 );
    lv_obj_set_event_cb( this->btn_join, GuiConnectivity::callback );
    lv_obj_set_user_data( this->btn_join, this );
    lv_obj_align( this->btn_join, NULL, LV_ALIGN_CENTER, 0, -60 );
    this->lbl_btn_join = lv_label_create( this->btn_join, NULL );
    lv_label_set_text( this->lbl_btn_join, "JOIN" );

    this->create_ddlist( &( this->ddlist_lorawan_region ), 140, "LoRaWAN region", "EU868\nUS915",
                         GuiConnectivity::callback_ddlist, 80 );

    this->create_ddlist( &( this->ddlist_adr_profile ), 180, "ADR profile", "NS controlled\nMobile LR\nMobile LP",
                         GuiConnectivity::callback_ddlist, 130 );

    if( this->_network_connectivity_settings->region == GUI_NETWORK_CONNECTIVITY_REGION_EU868 )
    {
        lv_ddlist_set_selected( this->ddlist_lorawan_region, 0 );
    }
    else if( this->_network_connectivity_settings->region == GUI_NETWORK_CONNECTIVITY_REGION_US915 )
    {
        lv_ddlist_set_selected( this->ddlist_lorawan_region, 1 );
    }

    if( this->_network_connectivity_settings->adr_profile == GUI_NETWORK_CONNECTIVITY_ADR_NETWORK_SERVER_CONTROLLED )
    {
        lv_ddlist_set_selected( this->ddlist_adr_profile, 0 );
    }
    else if( this->_network_connectivity_settings->adr_profile == GUI_NETWORK_CONNECTIVITY_ADR_MOBILE_LONG_RANGE )
    {
        lv_ddlist_set_selected( this->ddlist_adr_profile, 1 );
    }
    else if( this->_network_connectivity_settings->adr_profile == GUI_NETWORK_CONNECTIVITY_ADR_MOBILE_LOW_POWER )
    {
        lv_ddlist_set_selected( this->ddlist_adr_profile, 2 );
    }

    this->btn_restore = lv_btn_create( this->screen, NULL );
    lv_btn_set_state( this->btn_restore, LV_BTN_STATE_REL );
    lv_obj_set_height( this->btn_restore, 40 );
    lv_obj_set_width( this->btn_restore, 150 );
    lv_obj_set_event_cb( this->btn_restore, GuiConnectivity::callback );
    lv_obj_set_user_data( this->btn_restore, this );
    lv_obj_align( this->btn_restore, NULL, LV_ALIGN_CENTER, 0, 80 );
    this->lbl_btn_restore = lv_label_create( this->btn_restore, NULL );
    lv_label_set_text( this->lbl_btn_restore, "RESET EUI & KEYS" );

    this->createActionButton( &( this->btn_back ), "BACK", GuiConnectivity::callback, GUI_BUTTON_POS_CENTER, -5, true );

    this->updateNetworkConnectivityState( );

    lv_scr_load( this->screen );
}

GuiConnectivity::~GuiConnectivity( ) {}

void GuiConnectivity::updateNetworkConnectivityState( )
{
    switch( GuiCommon::_network_connectivity_status.connectivity_state )
    {
    case GUI_CONNECTIVITY_STATUS_NOT_CONNECTED:
    {
        lv_label_set_text( this->lbl_btn_join, "JOIN" );
        lv_label_set_text( this->lbl_connectivity_state, "State: Not connected" );
        lv_obj_set_click( this->ddlist_lorawan_region, true );
        break;
    }
    case GUI_CONNECTIVITY_STATUS_JOINING:
    {
        lv_label_set_text( this->lbl_btn_join, "ABORT" );
        lv_label_set_text( this->lbl_connectivity_state, "State: Joining..." );
        lv_obj_set_click( this->ddlist_lorawan_region, false );
        break;
    }
    case GUI_CONNECTIVITY_STATUS_CONNECTED:
    {
        lv_label_set_text( this->lbl_btn_join, "LEAVE" );
        lv_label_set_text( this->lbl_connectivity_state, "State: Connected" );
        lv_obj_set_click( this->ddlist_lorawan_region, false );
        break;
    }
    default:
        break;
    }
}

void GuiConnectivity::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiConnectivity* self = ( GuiConnectivity* ) lv_obj_get_user_data( obj );

    if( event == LV_EVENT_RELEASED )
    {
        if( obj == self->btn_join )
        {
            char text[10];
            strcpy( text, lv_label_get_text( self->lbl_btn_join ) );

            if( memcmp( text, "JOIN", 4 ) == 0 )
            {
                GuiCommon::_network_connectivity_status.connectivity_state = GUI_CONNECTIVITY_STATUS_JOINING;
                GuiCommon::_event                                          = GUI_EVENT_JOIN;
            }
            else if( memcmp( text, "ABORT", 4 ) == 0 )
            {
                GuiCommon::_network_connectivity_status.connectivity_state = GUI_CONNECTIVITY_STATUS_NOT_CONNECTED;
                GuiCommon::_event                                          = GUI_EVENT_ABORT;
            }
            else if( memcmp( text, "LEAVE", 4 ) == 0 )
            {
                GuiCommon::_network_connectivity_status.connectivity_state = GUI_CONNECTIVITY_STATUS_NOT_CONNECTED;
                GuiCommon::_event                                          = GUI_EVENT_LEAVE;
            }

            self->updateNetworkConnectivityIcon( self->_label_connectivity_icon );
            self->updateNetworkConnectivityState( );
        }
        else if( obj == self->btn_restore )
        {
            GuiCommon::_event = GUI_EVENT_RESTORE_EUI_KEYS;
            lv_btn_set_state( self->btn_restore, LV_BTN_STATE_INA );
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
            switch( id )
            {
            case 0:
            {
                self->_network_connectivity_settings->region = GUI_NETWORK_CONNECTIVITY_REGION_EU868;
                break;
            }
            case 1:
            {
                self->_network_connectivity_settings->region = GUI_NETWORK_CONNECTIVITY_REGION_US915;
                break;
            }
            default:
            {
                break;
            }
            }
        }
        else if( obj == self->ddlist_adr_profile )
        {
            switch( id )
            {
            case 0:
            {
                self->_network_connectivity_settings->adr_profile =
                    GUI_NETWORK_CONNECTIVITY_ADR_NETWORK_SERVER_CONTROLLED;
                break;
            }
            case 1:
            {
                self->_network_connectivity_settings->adr_profile = GUI_NETWORK_CONNECTIVITY_ADR_MOBILE_LONG_RANGE;
                break;
            }
            case 2:
            {
                self->_network_connectivity_settings->adr_profile = GUI_NETWORK_CONNECTIVITY_ADR_MOBILE_LOW_POWER;
                break;
            }
            default:
            {
                break;
            }
            }
        }
    }
}

void GuiConnectivity::create_ddlist( lv_obj_t** ddlist, int16_t off_y, const char* lbl_name, const char* options,
                                     lv_event_cb_t event_cb, int16_t width )
{
    lv_obj_t* lbl;

    lbl = lv_label_create( this->screen, NULL );
    lv_obj_set_style( lbl, &( GuiCommon::screen_style ) );
    lv_label_set_text( lbl, lbl_name );
    lv_obj_align( lbl, NULL, LV_ALIGN_IN_TOP_LEFT, 5, off_y );

    *ddlist = lv_ddlist_create( this->screen, NULL );
    lv_ddlist_set_fix_width( *ddlist, width );
    lv_ddlist_set_draw_arrow( *ddlist, true );
    lv_ddlist_set_selected( *ddlist, 0 );
    lv_obj_align( *ddlist, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, off_y );
    lv_obj_set_top( *ddlist, true );
    lv_obj_set_event_cb( *ddlist, event_cb );
    lv_ddlist_set_options( *ddlist, options );
    lv_obj_set_user_data( *ddlist, this );
}