#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "agentBase.hh"


CLICK_DECLS
AgentBase::AgentBase(){}

AgentBase::~AgentBase(){}

int AgentBase::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).read_mp("PUBADDR", public_addr).read_mp("PRIVADDR", private_addr).complete() < 0) return -1;

    maxRequests = 5;
	return 0;
}

bool AgentBase::inMapHome(IPAddress home){
    for(Vector<listItem>::iterator it = visitorMap.begin();it != visitorMap.end(); it++) {
        if (it->homeAgent == home){
            return true;
        }
    }
    return false;
}

bool AgentBase::inPendingHome(IPAddress home){
    for(Vector<listItem>::iterator it = registrationReq.begin();it != registrationReq.end(); it++) {
        if (it->homeAgent == home){
            return true;
        }
    }
    return false;
}

bool AgentBase::inMapNode(IPAddress node){
    for(Vector<listItem>::iterator it = visitorMap.begin();it != visitorMap.end(); it++) {
        if (it->ipSrc == node){
            return true;
        }
    }
    return false;
}

bool AgentBase::inPendingNode(IPAddress node){
    for(Vector<listItem>::iterator it = registrationReq.begin();it != registrationReq.end(); it++) {
        if (it->ipSrc == node){
            return true;
        }
    }
    return false;
}

bool AgentBase::isHome(IPAddress addr){
	if(!table.find_pair(addr)){
		return true;
	}
	return false;
}



CLICK_ENDDECLS
EXPORT_ELEMENT(AgentBase)
