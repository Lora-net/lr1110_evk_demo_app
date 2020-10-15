/**
 * @file      gui.cpp
 *
 * @brief     Implementation of the gui base class.
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

#include "gui.h"
#include "lvgl.h"

volatile bool Gui::interruptPending = false;
bool          Gui::isTouched        = false;

Gui::Gui( ) : refresh_pending( false ), event( GUI_LAST_EVENT_NONE )
{
    this->demo_results.wifi_result.nbMacAddrTotal = 0;
    this->demo_results.wifi_result.powerConsTotal = 0;
}

Gui::~Gui( ) {}

void Gui::Init( GuiDemoSettings_t* settings, GuiDemoSettings_t* settings_default,
                GuiGnssDemoAssistancePosition_t* assistance_position,
                GuiGnssDemoAssistancePosition_t* assistance_position_default, version_handler_t* version_handler )
{
    this->demo_settings                             = *settings;
    this->demo_settings_default                     = *settings_default;
    this->gnss_assistance_position                  = *assistance_position;
    this->gnss_assistance_position_default          = *assistance_position_default;
    this->version_handler                           = version_handler;
    this->network_connectivity_settings.region      = GUI_NETWORK_CONNECTIVITY_REGION_EU868;
    this->network_connectivity_settings.adr_profile = GUI_NETWORK_CONNECTIVITY_ADR_NETWORK_SERVER_CONTROLLED;

    this->guiCurrent = new GuiSplashScreen( this->version_handler );
}

void Gui::Runtime( )
{
    guiEvent_t    event_from_display = GUI_EVENT_NONE;
    guiPageType_t next_page_type     = GUI_PAGE_NONE;

    this->event = GUI_LAST_EVENT_NONE;

    event_from_display = this->guiCurrent->getAndClearEvent( );

    if( event_from_display != GUI_EVENT_NONE )
    {
        switch( this->guiCurrent->getType( ) )
        {
        case GUI_PAGE_SPLASHSCREEN:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_NEXT:
                next_page_type = GUI_PAGE_MENU;
                break;
            case GUI_EVENT_ABOUT:
                next_page_type = GUI_PAGE_ABOUT;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_ABOUT:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_SPLASHSCREEN;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_MENU:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_LAUNCH_RADIO_TEST_MODE:
                next_page_type = GUI_PAGE_MENU_RADIO_TEST_MODES;
                break;
            case GUI_EVENT_LAUNCH_DEMO:
                next_page_type = GUI_PAGE_MENU_DEMO;
                break;
            case GUI_EVENT_LAUNCH_CONNECTIVITY:
                next_page_type = GUI_PAGE_CONNECTIVITY;
                break;
            case GUI_EVENT_EUI:
                next_page_type = GUI_PAGE_EUI;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_CONNECTIVITY:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_JOIN:
                this->event = GUI_LAST_EVENT_JOIN;
                break;
            case GUI_EVENT_ABORT:
                this->event = GUI_LAST_EVENT_LEAVE;
                break;
            case GUI_EVENT_LEAVE:
                this->event = GUI_LAST_EVENT_LEAVE;
                break;
            case GUI_EVENT_RESTORE_EUI_KEYS:
                this->event = GUI_LAST_EVENT_RESET_SEMTECH_DEFAULT_COMMISSIONING;
                break;
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_MENU;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_EUI:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_MENU;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_MENU_RADIO_TEST_MODES:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_START_TX_CW:
                next_page_type = GUI_PAGE_RADIO_TX_CW;
                break;
            case GUI_EVENT_START_PER_TX:
            case GUI_EVENT_START_PER_RX:
                next_page_type = GUI_PAGE_RADIO_PER;
                break;
            case GUI_EVENT_START_PING_PONG:
                next_page_type = GUI_PAGE_RADIO_PING_PONG;
                break;
            case GUI_EVENT_CONFIG:
                next_page_type = GUI_PAGE_RADIO_TEST_MODES_CONFIG;
                break;
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_MENU;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_RADIO_TEST_MODES_CONFIG:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_MENU_RADIO_TEST_MODES;
                break;
            case GUI_EVENT_SAVE:
                next_page_type = GUI_PAGE_MENU_RADIO_TEST_MODES;
                this->event    = GUI_LAST_EVENT_UPDATE_DEMO_RADIO;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_RADIO_TX_CW:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_START_TX_CW:
                this->guiCurrent->start( );
                this->event = GUI_LAST_EVENT_START_DEMO_TX_CW;
                break;
            case GUI_EVENT_STOP:
                this->guiCurrent->stop( );
                this->event = GUI_LAST_EVENT_STOP_DEMO;
                break;
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_MENU_RADIO_TEST_MODES;
                this->event    = GUI_LAST_EVENT_STOP_DEMO;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_RADIO_PER:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_START_PER_TX:
                this->guiCurrent->start( );
                this->event = GUI_LAST_EVENT_START_DEMO_PER_TX;
                break;
            case GUI_EVENT_START_PER_RX:
                this->guiCurrent->start( );
                this->event = GUI_LAST_EVENT_START_DEMO_PER_RX;
                break;
            case GUI_EVENT_STOP:
                this->guiCurrent->stop( );
                this->event = GUI_LAST_EVENT_STOP_DEMO;
                break;
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_MENU_RADIO_TEST_MODES;
                this->event    = GUI_LAST_EVENT_STOP_DEMO;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_RADIO_PING_PONG:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_START_PING_PONG:
                this->guiCurrent->start( );
                this->event = GUI_LAST_EVENT_START_DEMO_PING_PONG;
                break;
            case GUI_EVENT_STOP:
                this->guiCurrent->stop( );
                this->event = GUI_LAST_EVENT_STOP_DEMO;
                break;
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_MENU_RADIO_TEST_MODES;
                this->event    = GUI_LAST_EVENT_STOP_DEMO;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_MENU_DEMO:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_START_WIFI:
                next_page_type               = GUI_PAGE_WIFI_TEST;
                this->at_least_one_scan_done = false;
                break;
            case GUI_EVENT_START_GNSS_AUTONOMOUS:
                next_page_type               = GUI_PAGE_GNSS_AUTONOMOUS_TEST;
                this->at_least_one_scan_done = false;
                break;
            case GUI_EVENT_START_GNSS_ASSISTED:
                next_page_type               = GUI_PAGE_GNSS_ASSISTED_TEST;
                this->at_least_one_scan_done = false;
                break;
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_MENU;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_WIFI_TEST:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_START_WIFI:
                this->guiCurrent->start( );
                this->event                  = GUI_LAST_EVENT_START_DEMO_WIFI;
                this->at_least_one_scan_done = true;
                break;
            case GUI_EVENT_STOP:
                this->event = GUI_LAST_EVENT_STOP_DEMO;
                break;
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_MENU_DEMO;
                this->event    = GUI_LAST_EVENT_STOP_DEMO;
                break;
            case GUI_EVENT_RESULTS:
                next_page_type = GUI_PAGE_WIFI_RESULTS;
                break;
            case GUI_EVENT_SEND:
                this->event = GUI_LAST_EVENT_SEND;
                break;
            case GUI_EVENT_CONFIG:
                next_page_type = GUI_PAGE_WIFI_CONFIG;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_WIFI_RESULTS:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_WIFI_TEST;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_WIFI_CONFIG:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_WIFI_TEST;
                break;
            case GUI_EVENT_SAVE:
                next_page_type = GUI_PAGE_WIFI_TEST;
                this->event    = GUI_LAST_EVENT_UPDATE_DEMO_WIFI;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_GNSS_AUTONOMOUS_TEST:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_STOP:
                this->event = GUI_LAST_EVENT_STOP_DEMO;
                break;
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_MENU_DEMO;
                this->event    = GUI_LAST_EVENT_STOP_DEMO;
                break;
            case GUI_EVENT_START_GNSS_AUTONOMOUS:
                this->guiCurrent->start( );
                this->event                  = GUI_LAST_EVENT_START_DEMO_GNSS_AUTONOMOUS;
                this->at_least_one_scan_done = true;
                break;
            case GUI_EVENT_RESULTS:
                next_page_type = GUI_PAGE_GNSS_AUTONOMOUS_RESULTS;
                break;
            case GUI_EVENT_SEND:
                this->event = GUI_LAST_EVENT_SEND;
                break;
            case GUI_EVENT_CONFIG:
                next_page_type = GUI_PAGE_GNSS_AUTONOMOUS_CONFIG;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_GNSS_AUTONOMOUS_RESULTS:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_GNSS_AUTONOMOUS_TEST;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_GNSS_AUTONOMOUS_CONFIG:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_GNSS_AUTONOMOUS_TEST;
                break;
            case GUI_EVENT_SAVE:
                next_page_type = GUI_PAGE_GNSS_AUTONOMOUS_TEST;
                this->event    = GUI_LAST_EVENT_UPDATE_DEMO_GNSS_AUTONOMOUS;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_GNSS_ASSISTED_TEST:
            switch( event_from_display )
            {
            case GUI_EVENT_START_GNSS_ASSISTED:
                this->guiCurrent->start( );
                this->event                  = GUI_LAST_EVENT_START_DEMO_GNSS_ASSISTED;
                this->at_least_one_scan_done = true;
                break;
            case GUI_EVENT_STOP:
                this->event = GUI_LAST_EVENT_STOP_DEMO;
                break;
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_MENU_DEMO;
                this->event    = GUI_LAST_EVENT_STOP_DEMO;
                break;
            case GUI_EVENT_RESULTS:
                next_page_type = GUI_PAGE_GNSS_ASSISTED_RESULTS;
                break;
            case GUI_EVENT_SEND:
                this->event = GUI_LAST_EVENT_SEND;
                break;
            case GUI_EVENT_CONFIG:
                next_page_type = GUI_PAGE_GNSS_ASSISTED_CONFIG;
                break;
            case GUI_EVENT_ASSISTANCE_POSITION:
                next_page_type = GUI_PAGE_GNSS_ASSISTANCE_POSITION_CONFIG;
                break;
            default:
                break;
            }
            break;

        case GUI_PAGE_GNSS_ASSISTED_RESULTS:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_GNSS_ASSISTED_TEST;
                break;
            default:
                break;
            }
        }

        case GUI_PAGE_GNSS_ASSISTED_CONFIG:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_GNSS_ASSISTED_TEST;
                break;
            case GUI_EVENT_SAVE:
                next_page_type = GUI_PAGE_GNSS_ASSISTED_TEST;
                this->event    = GUI_LAST_EVENT_UPDATE_DEMO_GNSS_ASSISTED;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_GNSS_ASSISTANCE_POSITION_CONFIG:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_BACK:
                next_page_type = GUI_PAGE_GNSS_ASSISTED_TEST;
                break;
            case GUI_EVENT_SAVE:
                next_page_type = GUI_PAGE_GNSS_ASSISTED_TEST;
                this->event    = GUI_LAST_EVENT_UPDATE_DEMO_GNSS_ASSISTANCE_POSITION;
                break;
            default:
                break;
            }
            break;
        }

        default:
            break;
        }
    }

    this->CreateNewPage( next_page_type );

    if( this->refresh_pending == true )
    {
        this->refresh_pending = false;
        this->guiCurrent->refresh( );
    }

    lv_task_handler( );
}

void Gui::GetRadioSettings( GuiRadioSetting_t* settings ) { *settings = this->demo_settings.radio_settings; }

void Gui::GetWifiSettings( GuiWifiDemoSetting_t* settings ) { *settings = this->demo_settings.wifi_settings; }

void Gui::GetGnssAutonomousSettings( GuiGnssDemoSetting_t* settings )
{
    *settings = this->demo_settings.gnss_autonomous_settings;
}

void Gui::GetGnssAssistedSettings( GuiGnssDemoSetting_t* settings )
{
    *settings = this->demo_settings.gnss_assisted_settings;
}

void Gui::GetGnssAssistancePosition( GuiGnssDemoAssistancePosition_t* assistance_position )
{
    *assistance_position = this->gnss_assistance_position;
}

void Gui::GetNetworkConnectivitySettings( GuiNetworkConnectivitySettings_t* connectivity_settings )
{
    *connectivity_settings = this->network_connectivity_settings;
}

void Gui::EnableConnectivity( ) const { GuiCommon::_has_connectivity = true; }

void Gui::DisableConnectivity( ) const { GuiCommon::_has_connectivity = false; }

void Gui::InterruptHandler( bool is_down )
{
    interruptPending = true;
    Gui::isTouched   = is_down;
}

void Gui::HostConnectivityChange( bool is_connected ) { this->guiCurrent->updateHostConnectivityState( is_connected ); }

void Gui::NetworkConnectivityChange( const GuiNetworkConnectivityStatus_t* new_connectivity_status )
{
    this->guiCurrent->updateNetworkConnectivityState( new_connectivity_status );
}

GuiLastEvent_t Gui::GetLastEvent( )
{
    GuiLastEvent_t event = this->event;
    this->event          = GUI_LAST_EVENT_NONE;
    return event;
}

void Gui::UpdateWifiDemoResult( GuiWifiResult_t& demo_result )
{
    this->demo_results.wifi_result = demo_result;
    this->refresh_pending          = true;
}

void Gui::UpdateGnssDemoResult( GuiGnssResult_t& demo_result )
{
    this->demo_results.gnss_result = demo_result;
    this->refresh_pending          = true;
}

void Gui::UpdateRadioPingPongResult( GuiRadioPingPongResult_t& demo_result )
{
    this->demo_results.radio_pingpong_result = demo_result;
    this->refresh_pending                    = true;
}

void Gui::UpdateRadioPerResult( GuiRadioPerResult_t& demo_result )
{
    this->demo_results.radio_per_result = demo_result;
    this->refresh_pending               = true;
}

void Gui::UpdateReverseGeoCoding( const GuiResultGeoLoc_t& new_reverse_geo_coding )
{
    this->demo_results.gnss_result.reverse_geo_loc = new_reverse_geo_coding;
    this->demo_results.wifi_result.reverse_geo_loc = new_reverse_geo_coding;
    this->refresh_pending                          = true;
}

void Gui::SetDemoStatus( GuiDemoStatus_t& demo_status ) {}

bool Gui::HasRefreshPending( ) const { return this->refresh_pending; }

void Gui::CreateNewPage( guiPageType_t page_type )
{
    if( page_type != GUI_PAGE_NONE )
    {
        delete this->guiCurrent;
        this->guiCurrent = NULL;

        switch( page_type )
        {
        case GUI_PAGE_SPLASHSCREEN:
        {
            this->guiCurrent = new GuiSplashScreen( this->version_handler );
            break;
        }
        case GUI_PAGE_ABOUT:
        {
            this->guiCurrent = new GuiAbout( this->version_handler );
            break;
        }
        case GUI_PAGE_MENU:
        {
            this->guiCurrent = new GuiMenu( this->version_handler );
            break;
        }
        case GUI_PAGE_CONNECTIVITY:
        {
            this->guiCurrent = new GuiConnectivity( &( this->network_connectivity_settings ) );
            break;
        }
        case GUI_PAGE_MENU_DEMO:
        {
            this->guiCurrent = new GuiMenuDemo( this->version_handler );
            break;
        }
        case GUI_PAGE_MENU_RADIO_TEST_MODES:
        {
            this->guiCurrent = new GuiMenuRadioTestModes( this->version_handler );
            break;
        }
        case GUI_PAGE_EUI:
        {
            this->guiCurrent = new GuiEui( this->version_handler );
            break;
        }
        case GUI_PAGE_WIFI_TEST:
        {
            this->guiCurrent = new GuiTestWifi( &demo_results.wifi_result, this->at_least_one_scan_done );
            break;
        }
        case GUI_PAGE_WIFI_CONFIG:
        {
            this->guiCurrent = new GuiConfigWifi( &( this->demo_settings.wifi_settings ),
                                                  &( this->demo_settings_default.wifi_settings ) );
            break;
        }
        case GUI_PAGE_WIFI_RESULTS:
        {
            this->guiCurrent = new GuiResultsWifi( &demo_results.wifi_result );
            break;
        }
        case GUI_PAGE_GNSS_AUTONOMOUS_TEST:
        {
            this->guiCurrent = new GuiTestGnss( &demo_results.gnss_result, GUI_PAGE_GNSS_AUTONOMOUS_TEST,
                                                this->at_least_one_scan_done );
            break;
        }
        case GUI_PAGE_GNSS_AUTONOMOUS_CONFIG:
        {
            this->guiCurrent =
                new GuiConfigGnss( GUI_PAGE_GNSS_AUTONOMOUS_CONFIG, &demo_settings.gnss_autonomous_settings,
                                   &demo_settings_default.gnss_autonomous_settings, this->version_handler );
            break;
        }
        case GUI_PAGE_GNSS_AUTONOMOUS_RESULTS:
        {
            this->guiCurrent = new GuiResultsGnss( &demo_results.gnss_result, GUI_PAGE_GNSS_AUTONOMOUS_RESULTS );
            break;
        }
        case GUI_PAGE_GNSS_ASSISTED_TEST:
        {
            this->guiCurrent =
                new GuiTestGnss( &demo_results.gnss_result, GUI_PAGE_GNSS_ASSISTED_TEST, this->at_least_one_scan_done );
            break;
        }
        case GUI_PAGE_GNSS_ASSISTED_CONFIG:
        {
            this->guiCurrent =
                new GuiConfigGnss( GUI_PAGE_GNSS_ASSISTED_CONFIG, &demo_settings.gnss_assisted_settings,
                                   &demo_settings_default.gnss_assisted_settings, this->version_handler );
            break;
        }
        case GUI_PAGE_GNSS_ASSISTED_RESULTS:
        {
            this->guiCurrent = new GuiResultsGnss( &demo_results.gnss_result, GUI_PAGE_GNSS_ASSISTED_RESULTS );
            break;
        }
        case GUI_PAGE_GNSS_ASSISTANCE_POSITION_CONFIG:
        {
            this->guiCurrent = new GuiConfigGnssAssistancePosition( &( this->gnss_assistance_position ),
                                                                    &( this->gnss_assistance_position_default ) );
            break;
        }
        case GUI_PAGE_RADIO_TEST_MODES_CONFIG:
        {
            this->guiCurrent = new GuiConfigRadioTestModes(
                &demo_settings.radio_settings, &demo_settings_default.radio_settings, this->version_handler );
            break;
        }
        case GUI_PAGE_RADIO_TX_CW:
        {
            this->guiCurrent = new GuiRadioTxCw( &demo_settings.radio_settings );
            break;
        }
        case GUI_PAGE_RADIO_PER:
        {
            this->guiCurrent = new GuiRadioPer( &demo_settings.radio_settings, &demo_results.radio_per_result );
            break;
        }
        case GUI_PAGE_RADIO_PING_PONG:
        {
            this->guiCurrent =
                new GuiRadioPingPong( &demo_settings.radio_settings, &demo_results.radio_pingpong_result );
            break;
        }
        default:
        {
            break;
        }
        }
    }
}
