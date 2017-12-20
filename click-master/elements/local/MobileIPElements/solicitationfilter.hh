#ifndef CLICK_SOLICITATIONFILTER_HH
#define CLICK_SOLICITATIONFILTER_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include "agentBase.hh"


CLICK_DECLS


class SolicitationFilter: public Element {
    public:
        SolicitationFilter();
        ~SolicitationFilter();


        const char *class_name() const { return "SolicitationFilter"; }
        const char *port_count() const { return "1/2"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);



};


CLICK_ENDDECLS
#endif
