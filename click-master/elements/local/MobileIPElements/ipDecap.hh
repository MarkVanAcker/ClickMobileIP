#ifndef CLICK_IPDECAP_HH
#define CLICK_IPDECAP_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include "agentBase.hh"


CLICK_DECLS


class IpDecap: public Element {
    public:
        IpDecap();
        ~IpDecap();


        const char *class_name() const { return "IpDecap"; }
        const char *port_count() const { return "1/2"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);

    private:
        AgentBase* visitorList;

};


CLICK_ENDDECLS
#endif
