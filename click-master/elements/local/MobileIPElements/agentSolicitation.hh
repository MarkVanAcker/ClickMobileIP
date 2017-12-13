#ifndef CLICK_AGENTSOLICITATION_HH
#define CLICK_AGENTSOLICITATION_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>

CLICK_DECLS

class AgentSolicitation: public Element {
    public:
        AgentSolicitation();
        ~AgentSolicitation();

        const char *class_name() const { return "AgentSolicitation"; }
        const char *port_count() const { return "0-1/1"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);
        void run_timer(Timer*);
        Packet* makePacket();

    private:
        Timer _timer;

        int _interval;
        int _maxRetransmissions;
        IPAddress _address;


};

struct SollicitationPacketheader{

    /* part 1 icmp adv */
    uint8_t     type;      // 10 for Sollicitation
    uint8_t     code;      // 0 for normal routing
    uint16_t    checksum;  // checksum , could be 0
    uint32_t    res;       // 0, ignored on reception


};


CLICK_ENDDECLS
#endif
