#ifndef CLICK_FOREIGNAGENTUTIL_HH
#define CLICK_FOREIGNAGENTUTIL_HH

#include <click/element.hh>
#include <click/ipaddress.hh>


CLICK_DECLS

struct listItem {
    EtherAddress ethSrc;
    IPAddress ipSrc;
    IPAddress ipDst;
    uint16_t udpSrc;
    IPAddress homeAgent;
    uint32_t id1;
    uint32_t id2;
    uint16_t lifetimeReq; // requested lifetime at the registration time for the binding
    uint16_t lifetimeRem; // remaining lifetime for the pending request
};



CLICK_ENDDECLS

#endif
