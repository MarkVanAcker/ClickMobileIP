#ifndef CLICK_VISITORLIST_HH
#define CLICK_VISITORLIST_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/timer.hh>
#include <click/etheraddress.hh>
#include "foreignAgentUtil.hh"

CLICK_DECLS


class VisitorList: public Element {
public:

    VisitorList();
    ~VisitorList();

    // input packet that is encapsulated
    // output 0 encap not for this FA
    // output 1 encap for this agent


    const char *class_name() const { return "VisitorList"; }
    const char *port_count() const { return "1/2"; }

    int configure(Vector<String>&, ErrorHandler*);
    void push(int, Packet *p);
    void run_timer(Timer*);

    bool inMap(IPAddress);
    bool inPending(IPAddress);

    Timer _timer;
    Vector<listItem> _registrationReq;
    HashTable<IPAddress,listItem> _visitorMap;

};



CLICK_ENDDECLS

#endif
