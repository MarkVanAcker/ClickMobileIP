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
    if (Args(conf, this, errh).read_mp("PUBADDR", _public_addr).read_mp("PRIVADDR", _private_addr).complete() < 0) return -1;

    _maxRequests = 5;
	return 0;
}

bool AgentBase::inMapHome(IPAddress home){
    for(Vector<listItem>::iterator it = _visitorMap.begin();it != _visitorMap.end(); ++it) {
        if (it->homeAgent == home){
            return true;
        }
    }
    return false;
}

bool AgentBase::inPendingHome(IPAddress home){
    for(Vector<listItem>::iterator it = _registrationReq.begin();it != _registrationReq.end(); ++it) {
        if (it->homeAgent == home){
            return true;
        }
    }
    return false;
}

bool AgentBase::inMapNode(IPAddress node){
    for(Vector<listItem>::iterator it = _visitorMap.begin();it != _visitorMap.end(); ++it) {
        if (it->ipSrc == node){
            return true;
        }
    }
    return false;
}

bool AgentBase::inPendingNode(IPAddress node){
    for(Vector<listItem>::iterator it = _registrationReq.begin();it != _registrationReq.end(); ++it) {
        if (it->ipSrc == node){
            return true;
        }
    }
    return false;
}

bool AgentBase::isHome(IPAddress addr){
	if(!_table.find_pair(addr)){
		return true;
	}
	return false;
}

/**
// packet is ip
// output 0 encap not for this FA
// output 1 encap for this agent

void AgentBase::push(int, Packet *p){
    click_ip* Oiph = (click_ip*)p->data();
    if(Oiph->ip_p != 4 && Oiph->ip_dst != _coa){
        click_chatter("Pack recieved in VList that is not IP IN IP protocol or for this host");
        p->kill();
    }
    // packet is ip in ip for this fa
    // the packet is for someone in the AgentBase
    if (inMapHome(Oiph->ip_src)){
        output(0).push(p);
    }

}

**/



CLICK_ENDDECLS
EXPORT_ELEMENT(AgentBase)
