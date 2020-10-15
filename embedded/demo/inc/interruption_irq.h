/**
 * @file      interruption_irq.h
 *
 * @brief     Definition of the interruption for transceiver class.
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

#ifndef __INTERRUPTION_IRQ_H__
#define __INTERRUPTION_IRQ_H__

#include "interruption_interface.h"
#include "lr1110_system.h"

class InterruptionIrq : public InterruptionInterface
{
   public:
    InterruptionIrq( );
    virtual ~InterruptionIrq( );
    bool is_wifi_interruption( ) const override;
    bool is_gnss_interruption( ) const override;
    bool is_radio_interruption( ) const override;

    lr1110_system_irq_mask_t get_irq_mask( ) const;
    lr1110_system_stat1_t    get_stat_1( ) const;
    lr1110_system_stat2_t    get_stat_2( ) const;

    void set_irq_mask( uint32_t irq_mask );
    void set_stat_1( lr1110_system_stat1_t stat_1 );
    void set_stat_2( lr1110_system_stat2_t stat_2 );

   private:
    uint32_t              irq_mask;
    lr1110_system_stat1_t stat_1;
    lr1110_system_stat2_t stat_2;
};

#endif  // __INTERRUPTION_IRQ_H__