/**
 * @file      guiConfigGnss.cpp
 *
 * @brief     Implementation of the gui config GNSS page.
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

#include "guiConfigGnss.h"

GuiConfigGnss::GuiConfigGnss( guiPageType_t pageType, GuiGnssDemoSetting_t* settings_current,
                              const GuiGnssDemoSetting_t* settings_default, version_handler_t* version_handler )
    : GuiCommon( pageType ), settings_current( settings_current ), settings_default( settings_default )
{
    this->settings_temp = *( this->settings_current );

    this->createHeaderWithIcons( "GNSS CONFIGURATION" );

    this->createSection( "CONSTELLATIONS", -100 );

    this->btnm_constellations = lv_btnm_create( this->screen, NULL );
    lv_btnm_set_map( this->btnm_constellations, this->btnm_constellations_map );
    lv_btnm_set_btn_ctrl_all( this->btnm_constellations, LV_BTNM_CTRL_TGL_ENABLE );
    lv_obj_set_size( this->btnm_constellations, 220, 40 );
    lv_obj_align( this->btnm_constellations, NULL, LV_ALIGN_CENTER, 0, -60 );
    lv_obj_set_event_cb( this->btnm_constellations, GuiConfigGnss::callbackSettings );
    lv_obj_set_user_data( this->btnm_constellations, this );

    if( ( _pageType == GUI_PAGE_GNSS_ASSISTED_CONFIG ) ||
        ( ( _pageType == GUI_PAGE_GNSS_AUTONOMOUS_CONFIG ) &&
          ( ( version_handler->device_type == VERSION_DEVICE_TRANSCEIVER ) ) ) )
    {
        this->createSection( "SCAN PARAMETERS", -10 );
    }

    this->createChoiceSwitch( &( this->sw_scan_mode ), this->screen, "Single", "Double",
                              GuiConfigGnss::callbackSettings, 30,
                              ( version_handler->device_type == VERSION_DEVICE_MODEM ) ? false : true );

    this->createChoiceSwitch( &( this->sw_scan_option ), this->screen, "Low power", "Best effort",
                              GuiConfigGnss::callbackSettings, 70,
                              ( _pageType == GUI_PAGE_GNSS_AUTONOMOUS_CONFIG ) ? false : true );

    this->createActionButton( &( this->btn_cancel ), "CANCEL", GuiConfigGnss::callback, GUI_BUTTON_POS_LEFT, -5, true );

    this->createActionButton( &( this->btn_default ), "DEFAULT", GuiConfigGnss::callback, GUI_BUTTON_POS_CENTER, -5,
                              ( this->IsConfigTempEqualTo( this->settings_default ) == true ) ? false : true );

    this->createActionButton( &( this->btn_save ), "SAVE", GuiConfigGnss::callback, GUI_BUTTON_POS_RIGHT, -5,
                              ( this->IsConfigTempEqualTo( this->settings_current ) == true ) ||
                                      ( this->IsAtLeastOneConstellationSelected( ) == false )
                                  ? false
                                  : true );

    this->ConfigSettingsButton( );

    lv_scr_load( this->screen );
}

GuiConfigGnss::~GuiConfigGnss( ) {}

void GuiConfigGnss::ConfigSettingsButton( )
{
    if( this->settings_temp.is_beidou_enabled == true )
    {
        lv_btnm_set_btn_ctrl( this->btnm_constellations, 1, LV_BTNM_CTRL_TGL_STATE );
    }
    else
    {
        lv_btnm_clear_btn_ctrl( this->btnm_constellations, 1, LV_BTNM_CTRL_TGL_STATE );
    }

    if( this->settings_temp.is_gps_enabled == true )
    {
        lv_btnm_set_btn_ctrl( this->btnm_constellations, 0, LV_BTNM_CTRL_TGL_STATE );
    }
    else
    {
        lv_btnm_clear_btn_ctrl( this->btnm_constellations, 0, LV_BTNM_CTRL_TGL_STATE );
    }

    if( this->settings_temp.is_dual_scan_activated == true )
    {
        lv_sw_on( this->sw_scan_mode, LV_ANIM_OFF );
    }
    else
    {
        lv_sw_off( this->sw_scan_mode, LV_ANIM_OFF );
    }

    if( this->settings_temp.is_best_effort_activated == true )
    {
        lv_sw_on( this->sw_scan_option, LV_ANIM_OFF );
    }
    else
    {
        lv_sw_off( this->sw_scan_option, LV_ANIM_OFF );
    }
}

void GuiConfigGnss::ConfigActionButton( )
{
    lv_btn_set_state( this->btn_default, ( ( this->IsConfigTempEqualTo( this->settings_default ) == true ) )
                                             ? LV_BTN_STATE_INA
                                             : LV_BTN_STATE_REL );

    lv_btn_set_state( this->btn_save, ( ( this->IsConfigTempEqualTo( this->settings_current ) == true ) ||
                                        ( this->IsAtLeastOneConstellationSelected( ) == false ) )
                                          ? LV_BTN_STATE_INA
                                          : LV_BTN_STATE_REL );
}

bool GuiConfigGnss::IsAtLeastOneConstellationSelected( )
{
    if( this->settings_temp.is_gps_enabled )
    {
        return true;
    }

    if( this->settings_temp.is_beidou_enabled )
    {
        return true;
    }

    return false;
}

bool GuiConfigGnss::IsConfigTempEqualTo( const GuiGnssDemoSetting_t* settings_to_compare )
{
    if( this->settings_temp.is_beidou_enabled != settings_to_compare->is_beidou_enabled )
    {
        return false;
    }

    if( this->settings_temp.is_gps_enabled != settings_to_compare->is_gps_enabled )
    {
        return false;
    }

    if( this->settings_temp.is_dual_scan_activated != settings_to_compare->is_dual_scan_activated )
    {
        return false;
    }

    if( this->settings_temp.is_best_effort_activated != settings_to_compare->is_best_effort_activated )
    {
        return false;
    }

    return true;
}

void GuiConfigGnss::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiConfigGnss* self = ( GuiConfigGnss* ) lv_obj_get_user_data( obj );

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

void GuiConfigGnss::callbackSettings( lv_obj_t* obj, lv_event_t event )
{
    GuiConfigGnss* self = ( GuiConfigGnss* ) lv_obj_get_user_data( obj );

    if( event == LV_EVENT_VALUE_CHANGED )
    {
        if( obj == self->btnm_constellations )
        {
            uint16_t id;

            id = lv_btnm_get_active_btn( obj );

            switch( id )
            {
            case 0:
                self->settings_temp.is_gps_enabled = !self->settings_temp.is_gps_enabled;
                self->ConfigActionButton( );
                break;
            case 1:
                self->settings_temp.is_beidou_enabled = !self->settings_temp.is_beidou_enabled;
                self->ConfigActionButton( );
                break;
            default:
                break;
            }
        }
        else if( obj == self->sw_scan_mode )
        {
            self->settings_temp.is_dual_scan_activated = !self->settings_temp.is_dual_scan_activated;
            self->ConfigActionButton( );
        }
        else if( obj == self->sw_scan_option )
        {
            self->settings_temp.is_best_effort_activated = !self->settings_temp.is_best_effort_activated;
            self->ConfigActionButton( );
        }
    }
}
