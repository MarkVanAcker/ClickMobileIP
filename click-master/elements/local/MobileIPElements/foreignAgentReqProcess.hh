#ifndef CLICK_FOREIGNAGENTREQPROCESS_HH
#define CLICK_FOREIGNAGANTREQPROCESS_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>

CLICK_DECLS


class ForeignAgentReqProcess: public Element {
    public:
        ForeignAgentReqProcess();
        ~ForeignAgentReqProcess();

        const char *class_name() const { return "ForeignAgentReqProcess"; }
        const char *port_count() const { return "1/2"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);

        private:

            IPAddress _foreignAgent;
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

struct listItem {
    EtherAddress ethSrc;
    IPAddress ipSrc;
    IPAddress ipDst;
    uint16_t udpSrc;
    IPAddress homeAgent;
    uint32_t id1;
    uint32_t id2;
    uint16_t lifetimeReq; // requested lifetime at the registration time
    uint16_t lifetimeRem; // remaining lifetime
};



CLICK_ENDDECLS
#endif
