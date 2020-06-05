/**
 * @file      guiConfigWifi.cpp
 *
 * @brief     Implementation of the gui config Wi-Fi page.
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

#include "guiConfigWifi.h"

GuiConfigWifi::GuiConfigWifi( GuiWifiDemoSetting_t* settings_current, const GuiWifiDemoSetting_t* settings_default )
    : GuiCommon( GUI_PAGE_WIFI_CONFIG ), settings_current( settings_current ), settings_default( settings_default )
{
    this->createHeader( "Wi-Fi - CONFIGURATION" );

    this->createSection( "TYPES", -100 );
    this->btnm_types = lv_btnm_create( this->screen, NULL );
    lv_btnm_set_map( this->btnm_types, this->btnm_types_map );
    lv_btnm_set_btn_ctrl_all( this->btnm_types, LV_BTNM_CTRL_TGL_ENABLE );
    lv_btnm_set_one_toggle( this->btnm_types, true );
    lv_obj_set_size( this->btnm_types, 220, 40 );
    lv_obj_align( this->btnm_types, NULL, LV_ALIGN_CENTER, 0, -60 );
    lv_obj_set_event_cb( this->btnm_types, GuiConfigWifi::callbackSettings );
    lv_obj_set_user_data( this->btnm_types, this );

    this->createSection( "CHANNELS", -20 );
    this->btnm_channels = lv_btnm_create( this->screen, NULL );
    lv_btnm_set_map( this->btnm_channels, this->btnm_channels_map );
    lv_btnm_set_btn_ctrl_all( this->btnm_channels, LV_BTNM_CTRL_TGL_ENABLE );
    lv_btnm_set_btn_ctrl( this->btnm_channels, 14, LV_BTNM_CTRL_HIDDEN );
    lv_obj_set_size( this->btnm_channels, 220, 100 );
    lv_obj_align( this->btnm_channels, NULL, LV_ALIGN_CENTER, 0, 50 );
    lv_obj_set_event_cb( this->btnm_channels, GuiConfigWifi::callbackSettings );
    lv_obj_set_user_data( this->btnm_channels, this );

    this->createActionButton( &( this->btn_cancel ), "CANCEL", GuiConfigWifi::callback, GUI_BUTTON_POS_LEFT, -5, true );

    this->createActionButton( &( this->btn_default ), "DEFAULT", GuiConfigWifi::callback, GUI_BUTTON_POS_CENTER, -5,
                              true );

    this->createActionButton( &( this->btn_save ), "SAVE", GuiConfigWifi::callback, GUI_BUTTON_POS_RIGHT, -5, false );
}

GuiConfigWifi::~GuiConfigWifi( ) {}

void GuiConfigWifi::init( ) {}

void GuiConfigWifi::draw( )
{
    this->settings_temp = *( this->settings_current );

    this->ConfigActionButton( );
    this->ConfigSettingsButton( );

    lv_scr_load( this->screen );
}

void GuiConfigWifi::ConfigSettingsButton( )
{
    if( this->settings_temp.is_type_b == true )
    {
        lv_btnm_set_btn_ctrl( btnm_types, 0, LV_BTNM_CTRL_TGL_STATE );
        lv_btnm_clear_btn_ctrl( btnm_types, 1, LV_BTNM_CTRL_TGL_STATE );
        lv_btnm_clear_btn_ctrl( btnm_types, 2, LV_BTNM_CTRL_TGL_STATE );
        lv_btnm_clear_btn_ctrl( btnm_types, 3, LV_BTNM_CTRL_TGL_STATE );
    }
    else if( this->settings_temp.is_type_g == true )
    {
        lv_btnm_clear_btn_ctrl( btnm_types, 0, LV_BTNM_CTRL_TGL_STATE );
        lv_btnm_set_btn_ctrl( btnm_types, 1, LV_BTNM_CTRL_TGL_STATE );
        lv_btnm_clear_btn_ctrl( btnm_types, 2, LV_BTNM_CTRL_TGL_STATE );
        lv_btnm_clear_btn_ctrl( btnm_types, 3, LV_BTNM_CTRL_TGL_STATE );
    }
    else if( this->settings_temp.is_type_n == true )
    {
        lv_btnm_clear_btn_ctrl( btnm_types, 0, LV_BTNM_CTRL_TGL_STATE );
        lv_btnm_clear_btn_ctrl( btnm_types, 1, LV_BTNM_CTRL_TGL_STATE );
        lv_btnm_set_btn_ctrl( btnm_types, 2, LV_BTNM_CTRL_TGL_STATE );
        lv_btnm_clear_btn_ctrl( btnm_types, 3, LV_BTNM_CTRL_TGL_STATE );
    }
    else if( this->settings_temp.is_type_all == true )
    {
        lv_btnm_clear_btn_ctrl( btnm_types, 0, LV_BTNM_CTRL_TGL_STATE );
        lv_btnm_clear_btn_ctrl( btnm_types, 1, LV_BTNM_CTRL_TGL_STATE );
        lv_btnm_clear_btn_ctrl( btnm_types, 2, LV_BTNM_CTRL_TGL_STATE );
        lv_btnm_set_btn_ctrl( btnm_types, 3, LV_BTNM_CTRL_TGL_STATE );
    }

    // Configure channel map
    for( uint8_t index = 0; index < 14; index++ )
    {
        if( ( this->settings_temp.channel_mask & ( 1 << index ) ) != 0 )
        {
            lv_btnm_set_btn_ctrl( btnm_channels, index, LV_BTNM_CTRL_TGL_STATE );
        }
        else
        {
            lv_btnm_clear_btn_ctrl( btnm_channels, index, LV_BTNM_CTRL_TGL_STATE );
        }
    }
}

void GuiConfigWifi::ConfigActionButton( )
{
    lv_btn_set_state( this->btn_default, ( ( this->IsConfigTempEqualTo( this->settings_default ) == true ) )
                                             ? LV_BTN_STATE_INA
                                             : LV_BTN_STATE_REL );

    lv_btn_set_state( this->btn_save, ( ( this->IsConfigTempEqualTo( this->settings_current ) == true ) )
                                          ? LV_BTN_STATE_INA
                                          : LV_BTN_STATE_REL );
}

bool GuiConfigWifi::IsConfigTempEqualTo( const GuiWifiDemoSetting_t* settings_to_compare )
{
    if( this->settings_temp.channel_mask != settings_to_compare->channel_mask )
    {
        return false;
    }
    else if( this->settings_temp.is_type_all != settings_to_compare->is_type_all )
    {
        return false;
    }
    else if( this->settings_temp.is_type_b != settings_to_compare->is_type_b )
    {
        return false;
    }
    else if( this->settings_temp.is_type_g != settings_to_compare->is_type_g )
    {
        return false;
    }
    else if( this->settings_temp.is_type_n != settings_to_compare->is_type_n )
    {
        return false;
    }

    return true;
}

void GuiConfigWifi::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiConfigWifi* self = ( GuiConfigWifi* ) lv_obj_get_user_data( obj );

    if( ( event == LV_EVENT_RELEASED ) && ( lv_btn_get_state( obj ) != LV_BTN_STATE_INA ) )
    {
        if( obj == self->btn_cancel )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
        else if( obj == self->btn_default )
        {
            self->settings_temp = *( self->settings_default );
            self->ConfigSettingsButton( );
            self->ConfigActionButton( );
        }
        else if( obj == self->btn_save )
        {
            *self->settings_current = self->settings_temp;
            GuiCommon::_event       = GUI_EVENT_SAVE;
        }
    }
}

void GuiConfigWifi::callbackSettings( lv_obj_t* obj, lv_event_t event )
{
    GuiConfigWifi* self = ( GuiConfigWifi* ) lv_obj_get_user_data( obj );

    if( event == LV_EVENT_VALUE_CHANGED )
    {
        if( obj == self->btnm_types )
        {
            uint16_t id = lv_btnm_get_active_btn( obj );

            switch( id )
            {
            case 0:
                if( self->settings_temp.is_type_b == true )
                {
                    lv_btnm_clear_btn_ctrl( self->btnm_types, 0, LV_BTNM_CTRL_TGL_STATE );
                }
                else
                {
                    self->settings_temp.is_type_b = true;
                    if( self->settings_temp.is_type_g == true )
                    {
                        self->settings_temp.is_type_g = false;
                    }
                    else if( self->settings_temp.is_type_n == true )
                    {
                        self->settings_temp.is_type_n = false;
                    }
                    else if( self->settings_temp.is_type_all == true )
                    {
                        self->settings_temp.is_type_all = false;
                    }
                }
                self->ConfigActionButton( );
                break;
            case 1:
                if( self->settings_temp.is_type_g == true )
                {
                    lv_btnm_clear_btn_ctrl( self->btnm_types, 1, LV_BTNM_CTRL_TGL_STATE );
                }
                else
                {
                    self->settings_temp.is_type_g = true;
                    if( self->settings_temp.is_type_b == true )
                    {
                        self->settings_temp.is_type_b = false;
                    }
                    else if( self->settings_temp.is_type_n == true )
                    {
                        self->settings_temp.is_type_n = false;
                    }
                    else if( self->settings_temp.is_type_all == true )
                    {
                        self->settings_temp.is_type_all = false;
                    }
                }
                self->ConfigActionButton( );
                break;
            case 2:
                if( self->settings_temp.is_type_n == true )
                {
                    lv_btnm_clear_btn_ctrl( self->btnm_types, 2, LV_BTNM_CTRL_TGL_STATE );
                }
                else
                {
                    self->settings_temp.is_type_n = true;
                    if( self->settings_temp.is_type_b == true )
                    {
                        self->settings_temp.is_type_b = false;
                    }
                    else if( self->settings_temp.is_type_g == true )
                    {
                        self->settings_temp.is_type_g = false;
                    }
                    else if( self->settings_temp.is_type_all == true )
                    {
                        self->settings_temp.is_type_all = false;
                    }
                }
                self->ConfigActionButton( );
                break;
            case 3:
                if( self->settings_temp.is_type_all == true )
                {
                    lv_btnm_clear_btn_ctrl( self->btnm_types, 3, LV_BTNM_CTRL_TGL_STATE );
                }
                else
                {
                    self->settings_temp.is_type_all = true;
                    if( self->settings_temp.is_type_b == true )
                    {
                        self->settings_temp.is_type_b = false;
                    }
                    else if( self->settings_temp.is_type_g == true )
                    {
                        self->settings_temp.is_type_g = false;
                    }
                    else if( self->settings_temp.is_type_n == true )
                    {
                        self->settings_temp.is_type_n = false;
                    }
                }
                self->ConfigActionButton( );
                break;
            default:
                break;
            }
        }
        else if( obj == self->btnm_channels )
        {
            uint16_t id = lv_btnm_get_active_btn( obj );

            if( ( ( self->settings_temp.channel_mask ) & ( 1 << id ) ) != 0 )
            {
                self->settings_temp.channel_mask &= ~( 1 << id );
            }
            else
            {
                self->settings_temp.channel_mask |= ( 1 << id );
            }
            self->ConfigActionButton( );
        }
    }
}
