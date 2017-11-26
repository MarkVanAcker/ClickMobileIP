#ifndef CLICK_BINDINGSLIST_HH
#define CLICK_BINDINGSLIST_HH


#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>

CLICK_DECLS

//
// Created by root on 25.11.17.
//



struct HARegistrationEntry{

    IPAddress mobile_node_homadress;           // 3 for registration reply
    IPAddress mobile_node_coa;           // code 1 for a registration reply
    uint16_t lifetime;      // seconds for the message to expire
    uint32_t id1;           // used for matching Registration Reqsuest
    uint32_t id2;           // used for matching Registration Reqsuest

};

typedef HashMap<IPAddress, HARegistrationEntry*> RegistryTable;

class bindingsList: public Element {
public:
    bindingsList();
    ~bindingsList();

    const char *class_name() const { return "bindingsList"; }
    const char *port_count() const { return "0/0"; }

    int configure(Vector<String>&, ErrorHandler*);
    void addEntry(IPAddress,HARegistrationEntry*);
    HARegistrationEntry* getEntry(IPAddress);

private:

    HashMap<IPAddress, HARegistrationEntry*> _table;

};




CLICK_ENDDECLS
#endif //CLICKMOBILEIP_bindingsLis_HH
