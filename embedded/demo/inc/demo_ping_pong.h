/**
 * @file      demo_ping_pong.h
 *
 * @brief     Definition of Ping-Pong demo.
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

#ifndef __DEMO_PING_PONG_H__
#define __DEMO_PING_PONG_H__

#include "demo_base.h"
#include "configuration.h"
#include "environment_interface.h"
#include "log.h"

#define DEMO_PING_PONG_MAX_PAYLOAD_SIZE ( 255 )

typedef struct
{
    uint8_t size;
    uint8_t content[DEMO_PING_PONG_MAX_PAYLOAD_SIZE];
} demo_ping_pong_rf_payload_t;

typedef struct
{
    demo_ping_pong_rf_payload_t received_payload;
    int8_t                      rssi;
} demo_ping_pong_fetched_payload_t;

typedef enum
{
    DEMO_PING_PONG_STATUS_OK,
    DEMO_PING_PONG_STATUS_ERROR,
} demo_ping_pong_status_t;

typedef enum
{
    DEMO_PING_PONG_STATE_INIT,
    DEMO_PING_PONG_STATE_MASTER_WAIT_SEND_PING,
    DEMO_PING_PONG_STATE_MASTER_WAIT_SEND_PING_DONE,
    DEMO_PING_PONG_STATE_MASTER_WAIT_START_RECEIVE_PONG,
    DEMO_PING_PONG_STATE_MASTER_WAIT_RECEIVE_PONG,
    DEMO_PING_PONG_STATE_SLAVE_WAIT_START_RECEIVE_PING,
    DEMO_PING_PONG_STATE_SLAVE_WAIT_RECEIVE_PING,
    DEMO_PING_PONG_STATE_SLAVE_WAIT_SEND_PONG,
    DEMO_PING_PONG_STATE_SLAVE_WAIT_SEND_PONG_DONE,
    DEMO_PING_PONG_STATE_TERMINATED,
    DEMO_PING_PONG_STATE_ERROR,
} demo_ping_pong_state_t;

typedef enum
{
    DEMO_PING_PONG_MODE_SLAVE,
    DEMO_PING_PONG_MODE_MASTER,
} demo_ping_pong_mode_t;

typedef struct
{
    uint32_t                  count_rx_correct_packet;
    uint32_t                  count_rx_wrong_packet;
    uint32_t                  count_tx;
    uint32_t                  count_rx_timeout;
    lr1110_radio_stats_gfsk_t statistics_gfsk;
    lr1110_radio_stats_lora_t statistics_lora;
    demo_ping_pong_status_t   status;
    demo_ping_pong_mode_t     mode;
    int8_t                    last_rssi;
} demo_ping_pong_results_t;

class DemoPingPong : public DemoBase
{
   public:
    DemoPingPong( radio_t* radio, SignalingInterface* signaling, EnvironmentInterface* environment );
    virtual ~DemoPingPong( );

    void                            Configure( demo_ping_pong_settings_t& settings );
    const demo_ping_pong_results_t* GetResult( ) const;

   protected:
    virtual void SpecificRuntime( );
    virtual void SpecificStop( );
    virtual void SpecificInterruptHandler( );
    virtual void ClearRegisteredIrqs( ) const;
    void         LogInfo( ) const;

    demo_ping_pong_status_t ConfigureRadio( ) const;
    void                    StartSendMessage( ) const;
    void                    StartReceptionMessage( ) const;
    void                    EndReceptionMessage( ) const;
    void                    FetchPayload( demo_ping_pong_fetched_payload_t* fetch_payload ) const;
    void                    FetchStatisticToResults( );
    bool                    IsPongPayload( const demo_ping_pong_rf_payload_t& payload ) const;
    bool                    IsPingPayload( const demo_ping_pong_rf_payload_t& payload ) const;
    bool                    HasIntermediateResults( ) const;

    static void TransmitPayload( const void* radio, const uint8_t* payload, const uint8_t payload_size,
                                 const uint32_t timeout );

    static bool ArePayloadEquals( const demo_ping_pong_rf_payload_t& expected,
                                  const demo_ping_pong_rf_payload_t& test );

    static const char* ModeToString( const demo_ping_pong_mode_t mode );

   private:
    EnvironmentInterface*            environment;
    demo_ping_pong_mode_t            mode;
    demo_ping_pong_state_t           state;
    volatile uint32_t                last_irq_received_instant_ms;
    uint32_t                         last_tx_done_instant_ms;
    uint32_t                         last_rx_done_instant_ms;
    demo_ping_pong_results_t         results;
    demo_ping_pong_settings_t        settings;
    demo_ping_pong_rf_payload_t      payload_ping;
    demo_ping_pong_rf_payload_t      payload_pong;
    demo_ping_pong_fetched_payload_t received_payload;
    uint32_t                         radio_interrupt_mask;
    Logging                          log;
    bool                             has_intermediate_results;
};

#endif  // __DEMO_PING_PONG_H__