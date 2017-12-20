#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "mobileIPfilter	.hh"


CLICK_DECLS
MobileIPFilter::MobileIPFilter()
{}

MobileIPFilter::~MobileIPFilter()
{}

int MobileIPFilter::configure(Vector<String> &conf, ErrorHandler *errh) {
		AgentBase* templist;
    if (Args(conf, this, errh).read("BINDING", ElementCastArg("bindingsList"), templist).complete() < 0) return -1;
    _agentbase = templist;

	return 0;
}



void MobileIPFilter::push(int, Packet *p) {

    
    output(0).push(p);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPFilter)
