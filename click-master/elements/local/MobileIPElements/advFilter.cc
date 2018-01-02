#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "advFilter.hh"

CLICK_DECLS
AdvFilter::AdvFilter()
{}

AdvFilter::~AdvFilter()
{}

int AdvFilter::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).complete() < 0) return -1;

	return 0;
}



void AdvFilter::push(int, Packet *p) {
    int packetsize = p->length();
		if(packetsize == sizeof(click_ip) + sizeof(AdvertisementPacketheader)){
			    click_ip* iph = (click_ip*)p->data();
                    if(iph->ip_p == 1 && iph->ip_dst == IPAddress("255.255.255.255")){
                        AdvertisementPacketheader* ah = (AdvertisementPacketheader*) (iph+1);
    					if(ah->typeEx == 16 && ah->type == 9 && ah->code == 0 ){
                            click_chatter("is mobile ip adv");
    						output(1).push(p);
    						return;
    					}else{
                            click_chatter("Advertisement -> code or type (or both) not as expected");
                            return;
                        }
                    }

		}
    output(0).push(p);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(AdvFilter)
