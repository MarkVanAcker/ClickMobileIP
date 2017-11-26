//
// Created by root on 25.11.17.
//

#include "bindingsList.hh"
#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>

#include "foreignAgentReqProcess.hh"
#include "registrationRequestSource.hh"

CLICK_DECLS

int bindingsList::configure(Vector<String>&, ErrorHandler*){
    return 0;
}

HARegistrationEntry * bindingsList::getEntry(IPadress ip) {
    return _table.find_pair(ip)->value;
    //assert(pair)??
}

void bindingsList::addEntry(IPAdress ip, HARegistrationEntry * e) {
    _table.insert(ip,e);
}

CLICK_ENDDECLS

EXPORT_ELEMENT(bindingsList)