#ifndef CLICK_AGENTSOLICITATION_HH
#define CLICK_AGENTSOLICITATION_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include "mobileinfolist.hh"

CLICK_DECLS

class AgentSolicitation: public Element {
    public:
        AgentSolicitation();
        ~AgentSolicitation();

        const char *class_name() const { return "AgentSolicitation"; }
        const char *port_count() const { return "0-1/0-1"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);
        void run_timer(Timer*);
        Packet* makePacket();

    private:
        Timer _timer;

        MobileInfoList* _mobileNode;
        int _interval;
        int transmissions;
        int _maxRetransmissions;

};

struct SolicitationPacketheader{

    /* part 1 icmp adv */
    uint8_t     type;      // 10 for Sollicitation
    uint8_t     code;      // 0 for normal routing
    uint16_t    checksum;  // checksum , could be 0
    uint32_t    res;       // 0, ignored on reception


};


CLICK_ENDDECLS
#endif
