/**
 * @file      gui.h
 *
 * @brief     Definition of the gui base class.
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

#ifndef __GUI_H__
#define __GUI_H__

#include "guiCommon.h"
#include "guiFormat.h"
#include "guiMenu.h"
#include "guiConnectivity.h"
#include "guiMenuRadioTestModes.h"
#include "guiConfigRadioTestModes.h"
#include "guiRadioTxCw.h"
#include "guiRadioPer.h"
#include "guiRadioPingPong.h"
#include "guiMenuDemo.h"
#include "guiResultsWifi.h"
#include "guiResultsGnss.h"
#include "guiSplashScreen.h"
#include "guiAbout.h"
#include "guiTestWifi.h"
#include "guiTestGnss.h"
#include "guiConfigWifi.h"
#include "guiConfigGnss.h"
#include "guiConfigGnssAssistancePosition.h"
#include "guiEui.h"
#include <stdint.h>
#include <string.h>
#include "stdio.h"
#include "version.h"

typedef enum
{
    GUI_LAST_EVENT_NONE,
    GUI_LAST_EVENT_START_DEMO_TX_CW,
    GUI_LAST_EVENT_START_DEMO_PER_TX,
    GUI_LAST_EVENT_START_DEMO_PER_RX,
    GUI_LAST_EVENT_START_DEMO_PING_PONG,
    GUI_LAST_EVENT_START_DEMO_WIFI,
    GUI_LAST_EVENT_START_DEMO_GNSS_AUTONOMOUS,
    GUI_LAST_EVENT_START_DEMO_GNSS_ASSISTED,
    GUI_LAST_EVENT_STOP_DEMO,
    GUI_LAST_EVENT_SEND,
    GUI_LAST_EVENT_UPDATE_DEMO_RADIO,
    GUI_LAST_EVENT_UPDATE_DEMO_WIFI,
    GUI_LAST_EVENT_UPDATE_DEMO_GNSS_AUTONOMOUS,
    GUI_LAST_EVENT_UPDATE_DEMO_GNSS_ASSISTED,
    GUI_LAST_EVENT_UPDATE_DEMO_GNSS_ASSISTANCE_POSITION,
    GUI_LAST_EVENT_PRINT_EUI,
    GUI_LAST_EVENT_JOIN,
    GUI_LAST_EVENT_LEAVE,
    GUI_LAST_EVENT_RESET_SEMTECH_DEFAULT_COMMISSIONING,
} GuiLastEvent_t;

class Gui
{
   public:
    Gui( );
    virtual ~Gui( );

    virtual void Init( GuiDemoSettings_t* settings, GuiDemoSettings_t* settings_default,
                       GuiGnssDemoAssistancePosition_t* assistance_position,
                       GuiGnssDemoAssistancePosition_t* assistance_position_default,
                       version_handler_t*               version_handler );
    virtual void Runtime( );
    static void  InterruptHandler( bool is_down );

    void GetRadioSettings( GuiRadioSetting_t* settings );
    void GetWifiSettings( GuiWifiDemoSetting_t* settings );
    void GetGnssAutonomousSettings( GuiGnssDemoSetting_t* settings );
    void GetGnssAssistedSettings( GuiGnssDemoSetting_t* settings );
    void GetGnssAssistancePosition( GuiGnssDemoAssistancePosition_t* assistance_position );
    void GetNetworkConnectivitySettings( GuiNetworkConnectivitySettings_t* connectivity_settings );

    void EnableConnectivity( ) const;
    void DisableConnectivity( ) const;

    void HostConnectivityChange( bool is_connected );
    void NetworkConnectivityChange( const GuiNetworkConnectivityStatus_t* new_connectivity_status );

    virtual GuiLastEvent_t GetLastEvent( );
    void                   UpdateRadioPingPongResult( GuiRadioPingPongResult_t& gui_demo_result );
    void                   UpdateRadioPerResult( GuiRadioPerResult_t& demo_result );
    void                   UpdateWifiDemoResult( GuiWifiResult_t& gui_demo_result );
    void                   UpdateGnssDemoResult( GuiGnssResult_t& gui_demo_result );
    void                   UpdateReverseGeoCoding( const GuiResultGeoLoc_t& new_reverse_geo_coding );
    void                   SetDemoStatus( GuiDemoStatus_t& demo_status );
    bool                   HasRefreshPending( ) const;

    static const char* event2str( GuiLastEvent_t event )
    {
        switch( event )
        {
        case GUI_LAST_EVENT_NONE:
            return ( char* ) "None";
        case GUI_LAST_EVENT_START_DEMO_WIFI:
        case GUI_LAST_EVENT_START_DEMO_GNSS_AUTONOMOUS:
            return ( char* ) "Start demo";
        case GUI_LAST_EVENT_STOP_DEMO:
            return ( char* ) "Stop demo";
        case GUI_LAST_EVENT_SEND:
            return ( char* ) "Send";
        default:
            return ( char* ) "Unknown";
        }
    }

   protected:
    const GuiDemoResult_t&           GetDemoResult( ) const { return this->demo_results; }
    GuiDemoSettings_t                demo_settings;
    GuiDemoSettings_t                demo_settings_default;
    GuiGnssDemoAssistancePosition_t  gnss_assistance_position;
    GuiGnssDemoAssistancePosition_t  gnss_assistance_position_default;
    GuiNetworkConnectivitySettings_t network_connectivity_settings;
    bool                             at_least_one_scan_done;

   private:
    void                 CreateNewPage( guiPageType_t page_type );
    bool                 refresh_pending;
    static volatile bool interruptPending;
    static bool          isTouched;
    GuiLastEvent_t       event;
    GuiDemoResult_t      demo_results;
    version_handler_t*   version_handler;
    GuiCommon*           guiCurrent;
};

#endif  // __GUI_H__
