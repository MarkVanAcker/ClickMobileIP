//
// Created by root on 25.11.17.
//

#ifndef CLICKMOBILEIP_HOMEAGENTREGPROCESS_HH
#define CLICKMOBILEIP_HOMEAGENTREGPROCESS_HH


#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>

CLICK_DECLS


class homeAgentRegProcess: public Element {
public:
    homeAgentRegProcess();
    ~homeAgentRegProcess();

    const char *class_name() const { return "homeAgentRegProcess"; }
    const char *port_count() const { return "1/2"; }
    const char *processing() const { return PUSH; }

    int configure(Vector<String>&, ErrorHandler*);
    void push(int, Packet*);

private:

    HashMap<IPAddress*, HARegistrationEntry*> RegistryTable;


};

struct HARegistrationEntry {

    IPAdress *  mobile_node_homadress;           // 3 for registration reply
    IPAdress * mobile_node_coa;           // code 1 for a registration reply
    uint16_t lifetime;      // seconds for the message to expire
    uint32_t id1;           // used for matching Registration Reqsuest
    uint32_t id2;           // used for matching Registration Reqsuest

};



CLICK_ENDDECLS

#endif //CLICKMOBILEIP_HOMEAGENTREGPROCESS_HH
