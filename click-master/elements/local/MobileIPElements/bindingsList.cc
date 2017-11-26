//
// Created by root on 25.11.17.
//

#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>

#include "bindingsList.hh"


CLICK_DECLS
bindingsList::bindingsList(){}

bindingsList::~bindingsList(){}

int bindingsList::configure(Vector<String>& conf, ErrorHandler* errh){
if (Args(conf, this, errh).complete() < 0) return -1;
    return 0;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(bindingsList)
