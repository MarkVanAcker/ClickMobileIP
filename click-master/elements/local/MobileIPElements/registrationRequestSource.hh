#ifndef CLICK_REGISTRATIONREQUESTSOURCE_HH
#define CLICK_REGISTRATIONREQUESTSOURCE_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>

CLICK_DECLS


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
        Packet* makePacket();

    private:
        Timer _timer;
        bool _registrated;
        bool _remainingBidingLifetime;
        // default foreign agent time ? 1800

        IPAddress _homeAddress;
        IPAddress _homeAgent;
        IPAddress _CoA;

        /* securing registration request-replies */
        Vector<unsigned int> _identifications1;
        Vector<unsigned int> _identifications2;




};


struct RegistrationRequestPacketheader{

    uint8_t type; // Type 1 (Registration Request)
    uint8_t flags; // 8 flag bits
    uint16_t lifetime; // Seconds remaining before the registration is expired
    uint32_t homeAddr; // IP adress of the mobile node
    uint32_t homeAgent; // IP adress of the mobile node
    uint32_t coAddr; // IP adress of the end of the tunnel
    uint32_t id1; // used for matching Registration Reqsuest
    uint32_t id2; // used for matching Registration Reqsuest

    // no extensions needed
};




CLICK_ENDDECLS
#endif
