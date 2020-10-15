/**
 * @file      interruption_irq.cpp
 *
 * @brief     Implementation of the interruption for transceiver class.
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

#include "interruption_irq.h"

InterruptionIrq::InterruptionIrq( ) : irq_mask( 0 ), stat_1( ), stat_2( ) {}

InterruptionIrq::~InterruptionIrq( ) {}

bool InterruptionIrq::is_wifi_interruption( ) const
{
    return ( ( this->irq_mask & LR1110_SYSTEM_IRQ_WIFI_SCAN_DONE ) != 0 );
}

bool InterruptionIrq::is_gnss_interruption( ) const
{
    return ( ( this->irq_mask & LR1110_SYSTEM_IRQ_GNSS_SCAN_DONE ) != 0 );
}

bool InterruptionIrq::is_radio_interruption( ) const
{
    return ( ( this->irq_mask &
               ( LR1110_SYSTEM_IRQ_TX_DONE | LR1110_SYSTEM_IRQ_RX_DONE | LR1110_SYSTEM_IRQ_PREAMBLE_DETECTED |
                 LR1110_SYSTEM_IRQ_SYNC_WORD_HEADER_VALID | LR1110_SYSTEM_IRQ_HEADER_ERROR |
                 LR1110_SYSTEM_IRQ_CRC_ERROR | LR1110_SYSTEM_IRQ_CAD_DONE | LR1110_SYSTEM_IRQ_CAD_DETECTED |
                 LR1110_SYSTEM_IRQ_TIMEOUT | LR1110_SYSTEM_IRQ_FSK_LEN_ERROR | LR1110_SYSTEM_IRQ_FSK_ADDR_ERROR ) ) !=
             0 );
}

lr1110_system_irq_mask_t InterruptionIrq::get_irq_mask( ) const { return ( lr1110_system_irq_mask_t ) this->irq_mask; }

lr1110_system_stat1_t InterruptionIrq::get_stat_1( ) const { return this->stat_1; }

lr1110_system_stat2_t InterruptionIrq::get_stat_2( ) const { return this->stat_2; }

void InterruptionIrq::set_irq_mask( uint32_t irq_mask ) { this->irq_mask = irq_mask; }

void InterruptionIrq::set_stat_1( lr1110_system_stat1_t stat_1 ) { this->stat_1 = stat_1; }

void InterruptionIrq::set_stat_2( lr1110_system_stat2_t stat_2 ) { this->stat_2 = stat_2; }
