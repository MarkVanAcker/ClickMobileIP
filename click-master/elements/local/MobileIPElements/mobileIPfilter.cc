#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "mobileIPfilter.hh"
#include "registrationRequestSource.hh"




CLICK_DECLS
MobileIPFilter::MobileIPFilter()
{}

MobileIPFilter::~MobileIPFilter()
{}

int MobileIPFilter::configure(Vector<String> &conf, ErrorHandler *errh) {
		AgentBase* templist;
    if (Args(conf, this, errh).read_mp("AGBASE", ElementCastArg("AgentBase"), templist).complete() < 0) return -1;
		agentbase = templist;

	return 0;
}



void MobileIPFilter::push(int, Packet *p) {


	click_ip *iph = (click_ip*)p->data();
    click_udp *udph = (click_udp*)(iph+1);
	typepacket* m = (typepacket*)(udph+1);

	if (m->type == 3){

		output(1).push(p);
		return;
	}else if (m->type == 1){
		RegistrationRequestPacketheader *format = (RegistrationRequestPacketheader*)(udph+1);
		if(format->homeAgent == agentbase->public_addr){
			output(2).push(p);
		}else{
			output(3).push(p);
		}
		return;
	}else{
	output(0).push(p);
		return;
	}

}


CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPFilter)
