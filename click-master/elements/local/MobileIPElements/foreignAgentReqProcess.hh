#ifndef CLICK_FOREIGNAGENTREQPROCESS_HH
#define CLICK_FOREIGNAGANTREQPROCESS_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include "agentBase.hh"

CLICK_DECLS


class ForeignAgentReqProcess: public Element {
    public:
        ForeignAgentReqProcess();
        ~ForeignAgentReqProcess();

        const char *class_name() const { return "ForeignAgentReqProcess"; }
        const char *port_count() const { return "1/1-2"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);
        void run_timer(Timer*);


        private:


            Timer _timer;
            AgentBase* _visitorList;
            short int _maxLifetime; // ??

            unsigned short int makePacket(Packet *packet);
            unsigned short int validatePacket(Packet *packet);

};

struct ForeignAgentReqProcessPacketheader {

    uint8_t type;           // 3 for registration reply
    uint8_t code;           // code 1 for a registration reply
    uint16_t lifetime;      // seconds for the message to expire
    uint32_t homeAddr;      // IP adress of the mobile node
    uint32_t homeAgent;     // IP adress of the mobile node
    uint32_t id1;           // used for matching Registration Reqsuest
    uint32_t id2;           // used for matching Registration Reqsuest

    // no extensions needed
};




CLICK_ENDDECLS
#endif
