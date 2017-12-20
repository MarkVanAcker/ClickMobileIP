#ifndef CLICK_BINDINGSLIST_HH
#define CLICK_BINDINGSLIST_HH


#include <click/element.hh>
#include <click/ipaddress.hh>
#include <click/hashmap.hh>

CLICK_DECLS

//
// Created by root on 25.11.17.
//



struct HARegistrationEntry{

    IPAddress mobile_node_homadress;
    IPAddress mobile_node_coa;
    uint16_t lifetime;      // seconds for the message to expire
    uint32_t id1;           // used for matching Registration Reqsuest
    uint32_t id2;           // used for matching Registration Reqsuest

};

typedef HashMap<IPAddress, HARegistrationEntry*> RegistrationTable;
typedef Vector<IPAddress> RegistrationIPList;

class bindingsList: public Element {
public:
    bindingsList();
    ~bindingsList();

    const char *class_name() const { return "bindingsList"; }
    const char *port_count() const { return "0/0"; }

    int configure(Vector<String>&, ErrorHandler*);
    bool isHome(IPAddress);

    RegistrationTable _table;
	RegistrationIPList _list;

};




CLICK_ENDDECLS
#endif //CLICKMOBILEIP_bindingsLis_HH
