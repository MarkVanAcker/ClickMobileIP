#ifndef CLICK_REGISTRATIONREQUESTREPLY_HH
#define CLICK_REGISTRATIONREQUESTREPLY_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>

CLICK_DECLS


class RegistrationRequestReply: public Element {
    public:
        RegistrationRequestReply();
        ~RegistrationRequestReply();

        const char *class_name() const { return "RegistrationRequestReply"; }
        const char *port_count() const { return "1/1"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);

        private:
            IPAddress _homeAgent;
            unsigned short int validatePacket(Packet *packet);
};

struct RegistrationRequestReplyPacketheader {

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
