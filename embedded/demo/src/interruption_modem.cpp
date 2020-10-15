#include "interruption_modem.h"

InterruptionModem::InterruptionModem( ) : event( ) {}

bool InterruptionModem::is_wifi_interruption( ) const
{
    return this->event.event_type == LR1110_MODEM_LORAWAN_EVENT_WIFI_SCAN_DONE;
}

bool InterruptionModem::is_gnss_interruption( ) const
{
    return this->event.event_type == LR1110_MODEM_LORAWAN_EVENT_GNSS_SCAN_DONE;
}

bool InterruptionModem::is_radio_interruption( ) const
{
    // We set to true the radio interruption whatever the event was, expect if it was Wi-Fi or GNSS
    return ( !this->is_gnss_interruption( ) && !this->is_wifi_interruption( ) );
}

lr1110_modem_event_fields_t InterruptionModem::GetEvent( ) const { return this->event; }

void InterruptionModem::SetEvent( const lr1110_modem_event_fields_t& _event ) { this->event = _event; }
