#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "mobileIPfilter.hh"


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


		click_ip *iph = (click_ip*)p->data();
    click_udp *udph = (click_udp*)(iph+1);
		typepacket* m = (typepacket*)(udph+1);

		if ( m->type == 3){
			output(1).push(p);
			return;
		}else if (m->type == 1){
			output(2).push(p);
			return;
		}else{
    	output(0).push(p);
			return;
		}

}


CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPFilter)
