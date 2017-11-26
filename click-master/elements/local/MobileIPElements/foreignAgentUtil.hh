#ifndef CLICK_FOREIGNAGENTUTIL_HH
#define CLICK_FOREIGNAGENTUTIL_HH

CLICK_DECLS

struct listItem {
    EtherAddress ethSrc;
    IPAddress ipSrc;
    IPAddress ipDst;
    uint16_t udpSrc;
    IPAddress homeAgent; s
    uint64_t id;
    uint16_t lifetimeReq; // requested lifetime at the registration time
    uint16_t lifetimeRem; // remaining lifetime
};



CLICK_ENDDECLS

#endif
