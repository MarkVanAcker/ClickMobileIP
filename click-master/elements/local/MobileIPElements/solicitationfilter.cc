#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "solicitationfilter.hh"
#include "agentSolicitation.hh"

CLICK_DECLS
SolicitationFilter::SolicitationFilter()
{}

SolicitationFilter::~SolicitationFilter()
{}

int SolicitationFilter::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).complete() < 0) return -1;

	return 0;
}



void SolicitationFilter::push(int, Packet *p) {

    int packetsize = p->length();
		if(packetsize == sizeof(click_ip) + sizeof(SollicitationPacketheader)){
			    click_ip* iph = (click_ip*)p->data();
					SollicitationPacketheader* sh = (SollicitationPacketheader*) (iph+1);

					if(sh->type == 10 && sh->code == 0 && iph->ip_dst == IPAddress("255.255.255.255")){
						output(1).push(p);
						return;
					}
		}
    output(0).push(p);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(SolicitationFilter)
