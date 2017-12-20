#ifndef CLICK_ADVFILTER_HH
#define CLICK_ADVFILTER_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include "agentAdvertisements.hh"


CLICK_DECLS


class AdvFilter: public Element {
    public:
        AdvFilter();
        ~AdvFilter();


        const char *class_name() const { return "AdvFilter"; }
        const char *port_count() const { return "1/2"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);



};


CLICK_ENDDECLS
#endif
