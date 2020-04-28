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

void Gui::Init( GuiDemoSettings_t* settings, GuiDemoSettings_t* settings_default, version_handler_t* version_handler )
{
    this->demo_settings         = *settings;
    this->demo_settings_default = *settings_default;
    this->version_handler       = version_handler;

    guiPages.guiSplashscreen = new GuiSplashScreen( );

    guiPages.guiAbout = new GuiAbout( this->version_handler );

    guiPages.guiMenu = new GuiMenu( );

    guiPages.guiMenuRadioTestModes   = new GuiMenuRadioTestModes( );
    guiPages.guiConfigRadioTestModes = new GuiConfigRadioTestModes( &( this->demo_settings.radio_settings ),
                                                                    &( this->demo_settings_default.radio_settings ) );

    guiPages.guiMenuDemo = new GuiMenuDemo( );

    guiPages.guiCurrent = guiPages.guiSplashscreen;
    guiPages.guiNext    = guiPages.guiCurrent;
    guiPages.guiCurrent->draw( );

    this->demo_results.wifi_result.nbMacAddrTotal = 0;
}

void Gui::Runtime( )
{
    guiEvent_t event_from_display;

    event_from_display = guiPages.guiCurrent->getAndClearEvent( );

    if( event_from_display != GUI_EVENT_NONE )
    {
        switch( guiPages.guiCurrent->getType( ) )
        {
        case GUI_PAGE_SPLASHSCREEN:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_NEXT:
                delete guiPages.guiSplashscreen;
                delete guiPages.guiAbout;
                guiPages.guiSplashscreen = NULL;
                guiPages.guiAbout        = NULL;
                this->guiPages.guiNext   = this->guiPages.guiMenu;
                break;
            case GUI_EVENT_ABOUT:
                this->guiPages.guiNext = this->guiPages.guiAbout;
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
                this->guiPages.guiNext = this->guiPages.guiSplashscreen;
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
                this->guiPages.guiNext = this->guiPages.guiMenuRadioTestModes;
                break;
            case GUI_EVENT_LAUNCH_DEMO:
                this->guiPages.guiNext = this->guiPages.guiMenuDemo;
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
                this->guiPages.guiRadioTxCw = new GuiRadioTxCw( &( this->demo_settings.radio_settings ) );
                this->guiPages.guiNext      = this->guiPages.guiRadioTxCw;
                this->guiPages.guiNext->init( );
                break;
            case GUI_EVENT_START_PER_TX:
            case GUI_EVENT_START_PER_RX:
                this->guiPages.guiRadioPer = new GuiRadioPer( &( this->demo_settings.radio_settings ),
                                                              &( this->demo_results.radio_per_result ) );
                this->guiPages.guiNext     = this->guiPages.guiRadioPer;
                this->guiPages.guiNext->init( );
                break;
            case GUI_EVENT_START_PING_PONG:
                this->guiPages.guiRadioPingPong = new GuiRadioPingPong( &( this->demo_settings.radio_settings ),
                                                                        &( this->demo_results.radio_pingpong_result ) );
                this->guiPages.guiNext          = this->guiPages.guiRadioPingPong;
                this->guiPages.guiNext->init( );
                break;
            case GUI_EVENT_CONFIG:
                this->guiPages.guiNext = this->guiPages.guiConfigRadioTestModes;
                break;
            case GUI_EVENT_BACK:
                this->guiPages.guiNext = this->guiPages.guiMenu;
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
                this->guiPages.guiNext = this->guiPages.guiMenuRadioTestModes;
                break;
            case GUI_EVENT_SAVE:
                guiPages.guiNext = guiPages.guiMenuRadioTestModes;
                this->event      = GUI_LAST_EVENT_UPDATE_DEMO_RADIO;
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
                this->guiPages.guiCurrent->start( );
                this->event = GUI_LAST_EVENT_START_DEMO_TX_CW;
                break;
            case GUI_EVENT_STOP:
                this->guiPages.guiCurrent->stop( );
                this->event = GUI_LAST_EVENT_STOP_DEMO;
                break;
            case GUI_EVENT_BACK:
                delete this->guiPages.guiRadioTxCw;
                this->guiPages.guiRadioTxCw = NULL;
                this->guiPages.guiNext      = this->guiPages.guiMenuRadioTestModes;
                this->event                 = GUI_LAST_EVENT_STOP_DEMO;
                break;
            }
            break;
        }

        case GUI_PAGE_RADIO_PER:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_START_PER_TX:
                this->guiPages.guiCurrent->start( );
                this->event = GUI_LAST_EVENT_START_DEMO_PER_TX;
                break;
            case GUI_EVENT_START_PER_RX:
                this->guiPages.guiCurrent->start( );
                this->event = GUI_LAST_EVENT_START_DEMO_PER_RX;
                break;
            case GUI_EVENT_STOP:
                this->guiPages.guiCurrent->stop( );
                this->event = GUI_LAST_EVENT_STOP_DEMO;
                break;
            case GUI_EVENT_BACK:
                delete this->guiPages.guiRadioPer;
                this->guiPages.guiRadioPer = NULL;
                this->guiPages.guiNext     = this->guiPages.guiMenuRadioTestModes;
                this->event                = GUI_LAST_EVENT_STOP_DEMO;
                break;
            }
            break;
        }

        case GUI_PAGE_RADIO_PING_PONG:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_START_PING_PONG:
                this->guiPages.guiCurrent->start( );
                this->event = GUI_LAST_EVENT_START_DEMO_PING_PONG;
                break;
            case GUI_EVENT_STOP:
                this->guiPages.guiCurrent->stop( );
                this->event = GUI_LAST_EVENT_STOP_DEMO;
                break;
            case GUI_EVENT_BACK:
                delete this->guiPages.guiRadioPingPong;
                this->guiPages.guiRadioPingPong = NULL;
                this->guiPages.guiNext          = this->guiPages.guiMenuRadioTestModes;
                this->event                     = GUI_LAST_EVENT_STOP_DEMO;
                break;
            }
            break;
        }

        case GUI_PAGE_MENU_DEMO:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_START_WIFI:
                guiPages.guiTestWifi   = new GuiTestWifi( &demo_results.wifi_result );
                guiPages.guiResultWifi = new GuiResultsWifi( &demo_results.wifi_result );
                guiPages.guiConfigWifi = new GuiConfigWifi( &( this->demo_settings.wifi_settings ),
                                                            &( this->demo_settings_default.wifi_settings ) );
                this->guiPages.guiNext = this->guiPages.guiTestWifi;
                this->guiPages.guiNext->init( );
                break;
            case GUI_EVENT_START_GNSS_AUTONOMOUS:
                guiPages.guiTestGnssAutonomous =
                    new GuiTestGnss( &demo_results.gnss_result, GUI_PAGE_GNSS_AUTONOMOUS_TEST );
                guiPages.guiResultGnssAutonomous =
                    new GuiResultsGnss( &demo_results.gnss_result, GUI_PAGE_GNSS_AUTONOMOUS_RESULTS );
                guiPages.guiConfigGnssAutonomous = new GuiConfigGnss(
                    GUI_PAGE_GNSS_AUTONOMOUS_CONFIG, &( this->demo_settings.gnss_autonomous_settings ),
                    &( this->demo_settings_default.gnss_autonomous_settings ) );
                this->guiPages.guiNext = this->guiPages.guiTestGnssAutonomous;
                this->guiPages.guiNext->init( );
                break;
            case GUI_EVENT_START_GNSS_ASSISTED:
                guiPages.guiTestGnssAssisted =
                    new GuiTestGnss( &demo_results.gnss_result, GUI_PAGE_GNSS_ASSISTED_TEST );
                guiPages.guiResultGnssAssisted =
                    new GuiResultsGnss( &demo_results.gnss_result, GUI_PAGE_GNSS_ASSISTED_RESULTS );
                guiPages.guiConfigGnssAssisted =
                    new GuiConfigGnss( GUI_PAGE_GNSS_ASSISTED_CONFIG, &( this->demo_settings.gnss_assisted_settings ),
                                       &( this->demo_settings_default.gnss_assisted_settings ) );
                this->guiPages.guiNext = this->guiPages.guiTestGnssAssisted;
                this->guiPages.guiNext->init( );
                break;
            case GUI_EVENT_BACK:
                this->guiPages.guiNext = this->guiPages.guiMenu;
                break;
            default:
                this->event = GUI_LAST_EVENT_NONE;
                break;
            }
            break;
        }

        case GUI_PAGE_WIFI_TEST:
        {
            switch( event_from_display )
            {
            case GUI_EVENT_STOP:
                this->event = GUI_LAST_EVENT_STOP_DEMO;
                break;
            case GUI_EVENT_BACK:
                delete guiPages.guiTestWifi;
                delete guiPages.guiResultWifi;
                delete guiPages.guiConfigWifi;
                guiPages.guiTestWifi   = NULL;
                guiPages.guiResultWifi = NULL;
                guiPages.guiConfigWifi = NULL;
                guiPages.guiNext       = guiPages.guiMenuDemo;
                this->event            = GUI_LAST_EVENT_STOP_DEMO;
                break;
            case GUI_EVENT_START_WIFI:
                guiPages.guiCurrent->start( );
                this->event = GUI_LAST_EVENT_START_DEMO_WIFI;
                break;
            case GUI_EVENT_RESULTS:
                guiPages.guiNext = guiPages.guiResultWifi;
                break;
            case GUI_EVENT_SEND:
                this->event = GUI_LAST_EVENT_SEND;
                break;
            case GUI_EVENT_CONFIG:
                guiPages.guiNext = guiPages.guiConfigWifi;
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
                guiPages.guiNext = guiPages.guiTestWifi;
                break;
            case GUI_EVENT_LEFT:
            case GUI_EVENT_RIGHT:
                guiPages.guiCurrent->updateResults( event_from_display );
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
                guiPages.guiNext = guiPages.guiTestWifi;
                break;
            case GUI_EVENT_SAVE:
                guiPages.guiNext = guiPages.guiTestWifi;
                this->event      = GUI_LAST_EVENT_UPDATE_DEMO_WIFI;
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
                delete guiPages.guiTestGnssAutonomous;
                delete guiPages.guiResultGnssAutonomous;
                delete guiPages.guiConfigGnssAutonomous;
                guiPages.guiTestGnssAutonomous   = NULL;
                guiPages.guiResultGnssAutonomous = NULL;
                guiPages.guiConfigGnssAutonomous = NULL;
                this->event                      = GUI_LAST_EVENT_STOP_DEMO;
                guiPages.guiNext                 = guiPages.guiMenuDemo;
                break;
            case GUI_EVENT_START_GNSS_AUTONOMOUS:
                guiPages.guiCurrent->start( );
                this->event = GUI_LAST_EVENT_START_DEMO_GNSS_AUTONOMOUS;
                break;
            case GUI_EVENT_RESULTS:
                guiPages.guiNext = guiPages.guiResultGnssAutonomous;
                break;
            case GUI_EVENT_SEND:
                this->event = GUI_LAST_EVENT_SEND;
                break;
            case GUI_EVENT_CONFIG:
                guiPages.guiNext = guiPages.guiConfigGnssAutonomous;
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
                guiPages.guiNext = guiPages.guiTestGnssAutonomous;
                break;
            case GUI_EVENT_LEFT:
            case GUI_EVENT_RIGHT:
                guiPages.guiCurrent->updateResults( event_from_display );
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
                guiPages.guiNext = guiPages.guiTestGnssAutonomous;
                break;
            case GUI_EVENT_SAVE:
                guiPages.guiNext = guiPages.guiTestGnssAutonomous;
                this->event      = GUI_LAST_EVENT_UPDATE_DEMO_GNSS_AUTONOMOUS;
                break;
            default:
                break;
            }
            break;
        }

        case GUI_PAGE_GNSS_ASSISTED_TEST:
            switch( event_from_display )
            {
            case GUI_EVENT_STOP:
                this->event = GUI_LAST_EVENT_STOP_DEMO;
                break;
            case GUI_EVENT_BACK:
                delete guiPages.guiTestGnssAssisted;
                delete guiPages.guiResultGnssAssisted;
                delete guiPages.guiConfigGnssAssisted;
                guiPages.guiTestGnssAssisted   = NULL;
                guiPages.guiResultGnssAssisted = NULL;
                guiPages.guiConfigGnssAssisted = NULL;
                this->event                    = GUI_LAST_EVENT_STOP_DEMO;
                guiPages.guiNext               = guiPages.guiMenuDemo;
                break;
            case GUI_EVENT_START_GNSS_ASSISTED:
                guiPages.guiCurrent->start( );
                this->event = GUI_LAST_EVENT_START_DEMO_GNSS_ASSISTED;
                break;
            case GUI_EVENT_RESULTS:
                guiPages.guiNext = guiPages.guiResultGnssAssisted;
                break;
            case GUI_EVENT_SEND:
                this->event = GUI_LAST_EVENT_SEND;
                break;
            case GUI_EVENT_CONFIG:
                guiPages.guiNext = guiPages.guiConfigGnssAssisted;
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
                guiPages.guiNext = guiPages.guiTestGnssAssisted;
                break;
            case GUI_EVENT_LEFT:
            case GUI_EVENT_RIGHT:
                guiPages.guiCurrent->updateResults( event_from_display );
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
                guiPages.guiNext = guiPages.guiTestGnssAssisted;
                break;
            case GUI_EVENT_SAVE:
                guiPages.guiNext = guiPages.guiTestGnssAssisted;
                this->event      = GUI_LAST_EVENT_UPDATE_DEMO_GNSS_ASSISTED;
                break;
            default:
                break;
            }
            break;
        }

        default:
            break;
        }

        if( ( guiPages.guiCurrent == NULL ) || ( guiPages.guiCurrent->getType( ) != guiPages.guiNext->getType( ) ) )
        {
            guiPages.guiNext->draw( );
            guiPages.guiCurrent = guiPages.guiNext;
        }
    }

    if( this->refresh_pending == true )
    {
        this->refresh_pending = false;
        guiPages.guiCurrent->refresh( );
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

void Gui::InterruptHandler( bool is_down )
{
    interruptPending = true;
    Gui::isTouched   = is_down;
}

void Gui::HostConnectivityChange( bool is_connected )
{
    guiPages.guiCurrent->updateHostConnectivityState( is_connected );
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
