#ifndef CLICK_SETGATEWAY_HH
#define CLICK_SETGATEWAY_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include "mobileinfolist.hh"

CLICK_DECLS

class SetGateway: public Element {
    public:
        SetGateway();
        ~SetGateway();

        const char *class_name() const { return "SetGateway"; }
        const char *port_count() const { return "1/1"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);



    private:

        MobileInfoList* mobileNode;


};



CLICK_ENDDECLS
#endif
