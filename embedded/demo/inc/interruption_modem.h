#ifndef __INTERRUPTION_MODEM_H__
#define __INTERRUPTION_MODEM_H__

#include "interruption_interface.h"
#include "lr1110_modem_lorawan.h"

class InterruptionModem : public InterruptionInterface
{
   public:
    InterruptionModem( );
    bool is_wifi_interruption( ) const override;
    bool is_gnss_interruption( ) const override;
    bool is_radio_interruption( ) const override;

    lr1110_modem_event_fields_t GetEvent( ) const;
    void                        SetEvent( const lr1110_modem_event_fields_t& _event );

   private:
    lr1110_modem_event_fields_t event;
};

#endif  // __INTERRUPTION_MODEM_H__