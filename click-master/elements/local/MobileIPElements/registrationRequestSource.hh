#ifndef CLICK_REGISTRATIONREQUESTSOURCE_HH
#define CLICK_REGISTRATIONREQUESTSOURCE_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include "mobileinfolist.hh"

CLICK_DECLS

struct Request{
    IPAddress ipDst;
    IPAddress COA;
    uint32_t id1;
    uint32_t id2;
    uint16_t requestedLifetime;
    uint16_t remainingLifetime;
    uint16_t port;
};


class RegistrationRequestSource: public Element {
    public:
        RegistrationRequestSource();
        ~RegistrationRequestSource();

        const char *class_name() const { return "RegistrationRequestSource"; }
        const char *port_count() const { return "0-1/1"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);
        void run_timer(Timer*);
        void makePacket(Advertisement a);

        Vector<Request> currentRequests;

    private:
        MobileInfoList* mobileNode;
        Vector<Timer*> timers;

};


struct RegistrationRequestPacketheader{

    uint8_t type; // Type 1 (Registration Request)
    uint8_t flags; // 8 flag bits
    uint16_t lifetime; // Seconds remaining before the registration is expired
    IPAddress homeAddr; // IP adress of the mobile node
    IPAddress homeAgent; // IP adress of HA
    IPAddress coAddr; // IP adress of the end of the tunnel
    uint32_t id1; // used for matching Registration Reqsuest
    uint32_t id2; // used for matching Registration Reqsuest

    // no extensions needed
};




CLICK_ENDDECLS
#endif
