#ifndef CLICK_AGENTADVERTISEMENTS_HH
#define CLICK_AGENTADVERTISEMENTS_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>

CLICK_DECLS

class AgentAdvertiser: public Element {
    public:
        AgentAdvertiser();
        ~AgentAdvertiser();

        const char *class_name() const { return "AgentAdvertiser"; }
        const char *port_count() const { return "0/1"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);
        void run_timer(Timer*);
        Packet* makePacket();

    private:
        Timer _timer;

        bool _HA;
        bool _FA;
        int _lifetimeReg;
        int _lifetimeAdv;
        int _interval;
        uint16_t _sequenceNum; // max 0xffff
        IPAddress _address;
        IPAddress _addressCO;


};

struct AdvertisementPacketheader{

    /* part 1 icmp adv */
    uint8_t     type;           // 9 for adveritesements
    uint8_t     code;           // 0 for normal routing
    uint16_t    checksum;       // checksum , could be 0
    uint8_t     addressNumbers; // number of addresses
    uint8_t     addrEntrySize;  // size 2 in total
    uint16_t    lifetime;       // The maximum length of time that the Advertisement is considered valid in the absence of further Adv. (rfc)
    IPAddress   address;        // router adress
    uint32_t    pref;           // 0

    /* part 2 Advertisements extension */
    uint8_t     typeEx;         // type 16
    uint8_t     length;         // length is 10 because only 1 address
    uint16_t    sequenceNum;    // in 0 -> 0xffff
    uint16_t    lifetimeEx;     // reg lifetime (longest agent is willing to accept in a request)
    uint8_t     flags;          // 8 flags
    uint8_t     reserved;       // 0
    IPAddress   addressEx;      // care off

};


CLICK_ENDDECLS
#endif
