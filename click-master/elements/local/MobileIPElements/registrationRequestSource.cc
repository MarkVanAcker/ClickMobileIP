#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>

#include "registrationRequestSource.hh"

CLICK_DECLS
RegistrationRequestSource::RegistrationRequestSource(): _registrated(false),_remainingBidingLifetime(0)
{}

RegistrationRequestSource::~RegistrationRequestSource()
{}

int RegistrationRequestSource::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).read_mp("HADDR", _homeAddress).read_mp("HAGENT", _homeAgent).read_mp("COA", _CoA).complete() < 0) return -1;

	Timer *timer = new Timer(this);
	timer->initialize(this);
	timer->schedule_after_msec(1000);
	return 0;
}

Packet* RegistrationRequestSource::makePacket(){
    // make the packet
    int packet_size = sizeof(struct RegistrationRequestPacketheader)+ sizeof(click_ip) + sizeof(click_udp);
    int headroom = sizeof(click_ether);
    WritablePacket *packet = Packet::make(headroom, 0, packet_size, 0);
    if(packet == 0) {
        click_chatter("Could not make packet");
        return 0;
    }
    memset(packet->data(), 0, packet->length());

	click_ip *iph = (click_ip *)packet->data();
    iph->ip_v = 4;
    iph->ip_hl = sizeof(click_ip) >> 2;
    iph->ip_len = htons(packet->length());
    iph->ip_id = htons(1);
    iph->ip_p = 17;
    iph->ip_ttl = 30;
    iph->ip_src = _homeAgent;
    iph->ip_dst = _homeAddress;
    iph->ip_sum = click_in_cksum((unsigned char *)iph, sizeof(click_ip));

    packet->set_dst_ip_anno(iph->ip_dst); //not sure why it is used

    click_udp *udph = (click_udp*)(iph+1);
    udph->uh_sport = htons(100);
    udph->uh_dport = htons(434);
    udph->uh_ulen = htons(packet->length()-sizeof(click_ip));


    RegistrationRequestPacketheader* format = (RegistrationRequestPacketheader*)(udph+1);
    format->type = 1; //fixed
    format->flags = 0; //all flags 0   ||  4, 8, 16, 32 ?
    format->lifetime = htons(10);
    format->homeAddr = _homeAddress;
    format->homeAgent = _homeAgent;
    format->coAddr = _CoA;
    unsigned int id1  = rand() % (2147483647);
    unsigned int id2  = rand() % (2147483647);
    format->id1 = htonl(id1); // max 32 bit number (unsigned) if we want to secure the messages
    format->id2 = htonl(id2);

    _identifications1.push_back(id1); // indexwise connection
    _identifications2.push_back(id2);
    udph->uh_sum = click_in_cksum_pseudohdr(click_in_cksum((unsigned char*)udph, packet_size-sizeof(click_ip)),
    iph, packet_size - sizeof(click_ip));


    return packet;
}

void RegistrationRequestSource::push(int, Packet *p) {
    click_chatter("Could not make packet");
    Packet* requestSource = makePacket();
    output(0).push(requestSource);
    return;
}

void RegistrationRequestSource::run_timer(Timer *timer){

    click_chatter("timer goes off");
    Packet* requestSource = makePacket();
    output(0).push(requestSource);
    timer->reschedule_after_msec(1000);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RegistrationRequestSource)
