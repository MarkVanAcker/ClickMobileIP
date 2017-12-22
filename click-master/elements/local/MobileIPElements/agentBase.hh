#ifndef CLICK_AGENTBASE_HH
#define CLICK_AGENTBASE_HH


#include <click/element.hh>
#include <click/ipaddress.hh>
#include <click/hashmap.hh>
#include <click/timer.hh>
#include <click/etheraddress.hh>

CLICK_DECLS

struct listItem {
    EtherAddress ethSrc;
    IPAddress ipSrc;
    IPAddress ipDst;
    uint16_t udpSrc;
    uint16_t udpDst;
    IPAddress homeAgent;
    uint32_t id1;
    uint32_t id2;
    uint16_t lifetimeReq; // requested lifetime at the registration time
    uint16_t lifetimeRem; // remaining lifetime
};



struct HARegistrationEntry{

    IPAddress mobile_node_homadress;
    IPAddress mobile_node_coa;
    uint16_t lifetime;      // seconds for the message to expire
    uint32_t id1;           // used for matching Registration Reqsuest
    uint32_t id2;           // used for matching Registration Reqsuest

};

typedef HashMap<IPAddress, HARegistrationEntry*> RegistrationTable;
typedef Vector<IPAddress> RegistrationIPList;


class AgentBase: public Element {
public:
    AgentBase();
    ~AgentBase();

    const char *class_name() const { return "AgentBase"; }
    const char *port_count() const { return "0/0"; }
    int configure(Vector<String>&, ErrorHandler*);

    bool inMapHome(IPAddress);
    bool inPendingHome(IPAddress);
    bool inMapNode(IPAddress);
    bool inPendingNode(IPAddress);
    bool isHome(IPAddress);

    int maxRequests;
    IPAddress public_addr;
	IPAddress private_addr;
    Vector<listItem> registrationReq;
    Vector<listItem> visitorMap;

    RegistrationTable table;
    RegistrationIPList list;

};




CLICK_ENDDECLS
#endif //CLICKMOBILEIP_bindingsLis_HH
