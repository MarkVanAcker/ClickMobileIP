#ifndef CLICK_MOBILEIPFILTER_HH
#define CLICK_MOBILEIPFILTER_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include "agentBase.hh"


CLICK_DECLS


class MobileIPFilter: public Element {
    public:
        MobileIPFilter();
        ~MobileIPFilter();


        const char *class_name() const { return "MobileIPFilter"; }
        const char *port_count() const { return "1/3"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);

				AgentBase * _agentbase;



};

struct typepacket{
	uint8_t type;
};

CLICK_ENDDECLS
#endif
