/**
 * @file      supervisor.h
 *
 * @brief     Definition of the Supervisor class.
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

#ifndef __SUPERVISOR_H__
#define __SUPERVISOR_H__

#include "environment_interface.h"
#include "version.h"
#include "gui.h"
#include "log.h"
#include "configuration.h"
#include "demo.h"
#include "hci.h"
#include "command_factory.h"
#include "command_get_version.h"
#include "command_get_almanac_dates.h"
#include "command_configure.h"
#include "command_start_demo.h"
#include "command_fetch_result.h"
#include "command_set_date_loc.h"
#include "command_reset.h"
#include "command_update_almanac.h"
#include "command_check_almanac_update.h"

class Supervisor
{
   public:
    Supervisor( Gui* gui, radio_t* radio, Demo* demo, EnvironmentInterface* environment );
    virtual ~Supervisor( );

    void Init( );
    void Runtime( );
    void RuntimeAuto( );
    void SwitchInFieldTestMode( );
    bool CanEnterLowPower( ) const;

    static void InterruptHandlerGui( bool is_down );
    static void InterruptHandlerDemo( );

    bool HasPendingInterrupt( ) const;

   protected:
    void TransfertDemoResultsToGui( );
    void TransferResultToGui( const demo_wifi_scan_all_results_t* result );
    void TransferResultToGui( const demo_gnss_all_results_t* result );
    void TransferResultToGui( const demo_ping_pong_results_t* result );
    void TransferResultToGui( const demo_radio_per_results_t* result );

    void TransferResultToSerial( const demo_wifi_scan_all_results_t* result );
    void TransferResultToSerial( const demo_gnss_all_results_t* result );

    void ConvertSettingsFromDemoToGui( const demo_all_settings_t* demo_settings, GuiDemoSettings_t* gui_demo_settings );

    void ConvertSettingsFromGuiToDemo( const GuiRadioSetting_t* gui_settings, demo_radio_settings_t* demo_settings );
    void ConvertSettingsFromGuiToDemo( const GuiWifiDemoSetting_t* gui_settings, demo_wifi_settings_t* demo_settings );
    void ConvertSettingsFromGuiToDemo( const GuiGnssDemoSetting_t* gui_settings, demo_gnss_settings_t* demo_settings );

    void GuiRuntimeAndProcess( );
    void GuiRuntimeAndProcessAuto( );
    void DemoRuntimeAndProcess( );
    void HciRuntimeAndProces( );
    void TestHostRuntime( );

    void GetAndPropagateVersion( );

    void EnterWaitForInterrupt( ) const;

    static const char*     WifiTypeToStr( const lr1110_wifi_signal_type_result_t type );
    static GuiDemoStatus_t DemoGnssErrorCodeToGuiStatus( const demo_gnss_error_t error_code );

   private:
    static bool           is_interrupt_raised;
    bool                  run_demo;
    Demo*                 demo;
    Gui*                  gui;
    Logging               log;
    EnvironmentInterface* environment;
    radio_t*              radio;
    CommandFactory        command_factory;
    Hci                   hci;
    LoggingHostType_t     host_type;
    version_handler_t     version_handler;

    // Commands for HCI
    CommandGetVersion         com_get_version;
    CommandGetAlmanacDates    com_get_almanac_dates;
    CommandConfigure          com_configure;
    CommandStartDemo          com_start;
    CommandFetchResult        com_fetch_result;
    CommandSetDateLoc         com_set_date_loc;
    CommandReset              com_reset;
    CommandUpdateAlmanac      com_update_almanac;
    CommandCheckAlmanacUpdate com_check_almanac_update;
};

#endif  // __SUPERVISOR_H__
