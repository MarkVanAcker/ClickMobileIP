#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "ipDecap.hh"

CLICK_DECLS
IpDecap::IpDecap()
{}

IpDecap::~IpDecap()
{}

int IpDecap::configure(Vector<String> &conf, ErrorHandler *errh) {
    AgentBase* templist;
    if (Args(conf, this, errh).read_m("BASE",
    ElementCastArg("AgentBase"),
    templist).complete() < 0) return -1;

    _visitorList = templist;
	return 0;
}


void IpDecap::push(int, Packet *p) {
    WritablePacket* q = p->uniqueify();
    q->pull(sizeof(click_ip));
    output(1).push(q);
    return;

    // get the careoff adress of the bindingslist
    click_ip* iph = (click_ip*)p->data();
	if(iph->ip_p == 4){
        click_ip* iph2 = (click_ip*)(iph+1);
        if(_visitorList->inMapNode(iph2->ip_src)){
            WritablePacket* q = p->uniqueify();
            q->pull(sizeof(click_ip));
            output(1).push(q);
            return;
        }
	}
    output(0).push(p);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(IpDecap)
