/**
 * @file      main.cpp
 *
 * @brief     LR1110 EVK application entry point.
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

#include "configuration.h"
#include "device_transceiver.h"
#include "device_modem.h"
#include "lr1110_modem_lorawan.h"
#include "system.h"

#include "supervisor.h"
#include "environment_interface.h"
#include "antenna_selector_interface.h"
#include "signaling_interface.h"
#include "timer_interface_implementation.h"

#include "gui.h"
#include "demo_manager_transceiver.h"
#include "demo_manager_modem.h"

#include "connectivity_manager_transceiver.h"
#include "connectivity_manager_modem.h"

#include "hci.h"
#include "command_factory.h"
#include "command_get_version.h"
#include "command_get_almanac_dates.h"
#include "command_start_demo.h"
#include "command_fetch_result.h"
#include "command_set_date_loc.h"
#include "command_reset.h"
#include "command_update_almanac.h"
#include "command_check_almanac_update.h"

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

radio_t radio = {
    SPI1,
    { LR1110_NSS_PORT, LR1110_NSS_PIN },
    { LR1110_RESET_PORT, LR1110_RESET_PIN },
    { LR1110_IRQ_PORT, LR1110_IRQ_PIN },
    { LR1110_BUSY_PORT, LR1110_BUSY_PIN },
};

class Environment : public EnvironmentInterface
{
   public:
    virtual time_t GetLocalTimeSeconds( ) const { return system_time_GetTicker( ) / 1000; }
    virtual time_t GetLocalTimeMilliseconds( ) const { return system_time_GetTicker( ); }
};

class AntennaSelector : public AntennaSelectorInterface
{
   public:
    AntennaSelector( ) : AntennaSelectorInterface( ) {}
    virtual void SelectAntenna1( )
    {
        AntennaSelector::init_if_not( );

        system_gpio_set_pin_state( AntennaSelector::antenna_selector_ctrl, SYSTEM_GPIO_PIN_STATE_LOW );
        system_gpio_set_pin_state( AntennaSelector::antenna_selector_n_ctrl, SYSTEM_GPIO_PIN_STATE_HIGH );
    }

    virtual void SelectAntenna2( )
    {
        AntennaSelector::init_if_not( );

        system_gpio_set_pin_state( AntennaSelector::antenna_selector_ctrl, SYSTEM_GPIO_PIN_STATE_HIGH );
        system_gpio_set_pin_state( AntennaSelector::antenna_selector_n_ctrl, SYSTEM_GPIO_PIN_STATE_LOW );
    }

   protected:
    static void init_if_not( )
    {
        if( AntennaSelector::has_been_init )
        {
            return;
        }
        else
        {
            system_gpio_init_direction_state( AntennaSelector::antenna_selector_ctrl, SYSTEM_GPIO_PIN_DIRECTION_OUTPUT,
                                              SYSTEM_GPIO_PIN_STATE_LOW );
            system_gpio_init_direction_state( AntennaSelector::antenna_selector_n_ctrl,
                                              SYSTEM_GPIO_PIN_DIRECTION_OUTPUT, SYSTEM_GPIO_PIN_STATE_LOW );

            AntennaSelector::has_been_init = true;
        }
    }

   private:
    static bool   has_been_init;
    static gpio_t antenna_selector_ctrl;
    static gpio_t antenna_selector_n_ctrl;
};
bool   AntennaSelector::has_been_init           = false;
gpio_t AntennaSelector::antenna_selector_ctrl   = { ANTENNA_SWITCH_CTRL_PORT, ANTENNA_SWITCH_CTRL_PIN };
gpio_t AntennaSelector::antenna_selector_n_ctrl = { ANTENNA_SWITCH_N_CTRL_PORT, ANTENNA_SWITCH_N_CTRL_PIN };

class Signaling : public SignalingInterface
{
   public:
    explicit Signaling( const EnvironmentInterface* environment )
        : SignalingInterface( ),
          environment( environment ),
          do_monitor_tx( false ),
          turn_on_tx_instant_ms( 0 ),
          do_monitor_rx( false ),
          turn_on_rx_instant_ms( 0 )
    {
    }
    virtual ~Signaling( ) {}

    void Runtime( )
    {
        const uint32_t now_ms = this->environment->GetLocalTimeMilliseconds( );
        if( this->do_monitor_tx )
        {
            if( ( now_ms - this->turn_on_tx_instant_ms ) > DURATION_TX_ON_MS )
            {
                this->do_monitor_tx = false;
                system_gpio_set_pin_state( Signaling::led_tx, SYSTEM_GPIO_PIN_STATE_LOW );
            }
        }
        if( this->do_monitor_rx )
        {
            if( ( now_ms - this->turn_on_rx_instant_ms ) > Signaling::DURATION_RX_ON_MS )
            {
                this->do_monitor_rx = false;
                system_gpio_set_pin_state( Signaling::led_rx, SYSTEM_GPIO_PIN_STATE_LOW );
            }
        }
    }

    virtual void StartCapture( ) { system_gpio_set_pin_state( Signaling::led_scan, SYSTEM_GPIO_PIN_STATE_HIGH ); }
    virtual void StopCapture( ) { system_gpio_set_pin_state( Signaling::led_scan, SYSTEM_GPIO_PIN_STATE_LOW ); }
    virtual void Tx( )
    {
        this->do_monitor_tx         = true;
        this->turn_on_tx_instant_ms = this->environment->GetLocalTimeMilliseconds( );
        system_gpio_set_pin_state( Signaling::led_tx, SYSTEM_GPIO_PIN_STATE_HIGH );
    }
    virtual void Rx( )
    {
        this->do_monitor_rx         = true;
        this->turn_on_rx_instant_ms = this->environment->GetLocalTimeMilliseconds( );
        system_gpio_set_pin_state( Signaling::led_rx, SYSTEM_GPIO_PIN_STATE_HIGH );
    }
    virtual void StartContinuousTx( )
    {
        this->do_monitor_tx = false;
        this->do_monitor_rx = false;
        system_gpio_set_pin_state( Signaling::led_tx, SYSTEM_GPIO_PIN_STATE_HIGH );
    }
    virtual void StopContinuousTx( )
    {
        this->do_monitor_tx = false;
        this->do_monitor_rx = false;
        system_gpio_set_pin_state( Signaling::led_tx, SYSTEM_GPIO_PIN_STATE_LOW );
    }

   protected:
    static uint32_t DURATION_TX_ON_MS;
    static uint32_t DURATION_RX_ON_MS;

   private:
    static gpio_t               led_scan;
    static gpio_t               led_tx;
    static gpio_t               led_rx;
    const EnvironmentInterface* environment;
    bool                        do_monitor_tx;
    uint32_t                    turn_on_tx_instant_ms;
    bool                        do_monitor_rx;
    uint32_t                    turn_on_rx_instant_ms;
};
gpio_t   Signaling::led_scan          = { LR1110_LED_SCAN_PORT, LR1110_LED_SCAN_PIN };
gpio_t   Signaling::led_tx            = { LR1110_LED_TX_PORT, LR1110_LED_TX_PIN };
gpio_t   Signaling::led_rx            = { LR1110_LED_RX_PORT, LR1110_LED_RX_PIN };
uint32_t Signaling::DURATION_TX_ON_MS = 100;
uint32_t Signaling::DURATION_RX_ON_MS = 100;

int main( void )
{
    DeviceInterface*              device;
    DemoManagerInterface*         demo_manager;
    ConnectivityManagerInterface* connectivity_manager;

    system_init( );

    system_time_wait_ms( 100 );  // Added to avoid screen flickering during power-up

    lv_init( );
    lv_port_disp_init( );
    lv_port_indev_init( );

    Environment          environment;
    AntennaSelector      antenna_selector;
    Signaling            signaling( &environment );
    Gui                  gui;
    Timer                timer;
    DeviceTransceiver    device_transceiver( &radio );
    CommandFactory       command_factory;
    Hci                  hci( command_factory, environment );
    CommunicationManager communication_manager( &environment, &hci );

    environment_location_t default_location(
        { DEMO_ASSISTANCE_LOCATION_LATITUDE, DEMO_ASSISTANCE_LOCATION_LONGITUDE, DEMO_ASSISTANCE_LOCATION_ALTITUDE } );
    environment.SetLocation( default_location );

    system_gpio_set_pin_state( radio.reset, SYSTEM_GPIO_PIN_STATE_LOW );
    system_time_wait_ms( 1 );
    system_gpio_set_pin_state( radio.reset, SYSTEM_GPIO_PIN_STATE_HIGH );

    system_time_wait_ms( 500 );  // Added to let the radio perform it startup sequence

    if( system_gpio_get_pin_state( radio.busy ) == SYSTEM_GPIO_PIN_STATE_HIGH )
    {
        lr1110_modem_event_fields_t modem_event;

        device       = new DeviceModem( &radio );
        demo_manager = new DemoManagerModem( ( DeviceModem* ) device, &environment, &antenna_selector, &signaling,
                                             &timer, &communication_manager );
        connectivity_manager = new ConnectivityManagerModem( ( DeviceModem* ) device );

        while( system_gpio_get_pin_state( radio.irq ) == SYSTEM_GPIO_PIN_STATE_LOW )
        {
        };

        lr1110_modem_get_event( &radio, &modem_event );

        if( modem_event.event_type != LR1110_MODEM_LORAWAN_EVENT_RESET )
        {
            while( 1 )
                ;
        }
    }
    else
    {
        device       = new DeviceTransceiver( &radio );
        demo_manager = new DemoManagerTransceiver( ( DeviceTransceiver* ) device, &environment, &antenna_selector,
                                                   &signaling, &timer, &communication_manager );
        connectivity_manager = new ConnectivityManagerTransceiver( );
    }

    CommandGetVersion         com_get_version( hci );
    CommandGetAlmanacDates    com_get_almanac_dates( device, hci );
    CommandStartDemo          com_start( device, hci, *demo_manager );
    CommandFetchResult        com_fetch_result( hci, environment, *demo_manager );
    CommandSetDateLoc         com_set_date_loc( device, hci, environment );
    CommandReset              com_reset( device, hci );
    CommandUpdateAlmanac      com_update_almanac( device, hci );
    CommandCheckAlmanacUpdate com_check_almanac_update( device, hci );

    command_factory.AddCommandToPool( com_get_version );
    command_factory.AddCommandToPool( com_get_almanac_dates );
    command_factory.AddCommandToPool( com_start );
    command_factory.AddCommandToPool( com_fetch_result );
    command_factory.AddCommandToPool( com_set_date_loc );
    command_factory.AddCommandToPool( com_reset );
    command_factory.AddCommandToPool( com_update_almanac );
    command_factory.AddCommandToPool( com_check_almanac_update );

    Supervisor supervisor( &gui, device, demo_manager, &environment, &communication_manager, connectivity_manager );

    supervisor.Init( );
    com_get_version.SetVersion( supervisor.GetVersionHandler( ) );

    system_uart_flush( );

    while( 1 )
    {
        signaling.Runtime( );
        supervisor.Runtime( );
    };
}
