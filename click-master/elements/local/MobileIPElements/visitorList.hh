#ifndef CLICK_VISITORLIST_HH
#define CLICK_VISITORLIST_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/etheraddress.hh>
#include "foreignAgentUtil.hh"

CLICK_DECLS


class VisitorList: public Element {
public:

    VisitorList();
    ~VisitorList();

    // input packet that is encapsulated
    // output 0 encap for this agent


    const char *class_name() const { return "VisitorList"; }
    const char *port_count() const { return "1/1"; }
    const char *processing() const { return PUSH; }

    int configure(Vector<String>&, ErrorHandler*);
    void push(int, Packet *p);
    void run_timer(Timer*);

    bool inMapHome(IPAddress);
    bool inPendingHome(IPAddress);
    bool inMapNode(IPAddress);
    bool inPendingNode(IPAddress);

    int _maxRequests;
    IPAddress _coa;
    Timer _timer;
    Vector<listItem> _registrationReq;
    Vector<listItem> _visitorMap;

};

struct listItem {
    EtherAddress ethSrc;
    IPAddress ipSrc;
    IPAddress ipDst;
    uint16_t udpSrc;
    IPAddress homeAgent; s
    uint32_t id1;
    uint32_t id2;
    uint16_t lifetimeReq; // requested lifetime at the registration time
    uint16_t lifetimeRem; // remaining lifetime
};




CLICK_ENDDECLS
#endif
