#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "agentSolicitation.hh"

CLICK_DECLS
AgentSolicitation::AgentSolicitation(){
    srand(time(NULL));
}

AgentSolicitation::~AgentSolicitation() {}

int AgentSolicitation::configure(Vector<String> &conf,ErrorHandler *errh) {
    if (Args(conf, this, errh)
        .read_m("ADDRNODE", _address)
        .read_m("MAX", _maxRetransmissions)
        .complete() < 0) return -1;

    // should send Sollicitations
    if(_maxRetransmissions < 1){
        return -1;
    }


    Timer *timer = new Timer(this);
	timer->initialize(this);
	timer->schedule_after_msec(500);
	return 0;
}


// create adveritesements
Packet* AgentSolicitation::makePacket() {
    int packetsize = sizeof(click_ip) + sizeof(SollicitationPacketheader);
    int headroom = sizeof(click_ether);
    WritablePacket* packet = Packet::make(headroom, 0, packetsize, 0);
    if (packet == 0){
        click_chatter("Packet creating failed (agent adveritesement)");
        return 0;
    }
    memset(packet->data(), 0, packet->length());
    click_ip* iph = (click_ip*) packet->data();
    iph->ip_v = 4;
    iph->ip_hl = sizeof(click_ip) >> 2;
    iph->ip_len = htons(packet->length());
    iph->ip_ttl = 1; // TTL must be 1 in Sollicitation
    iph->ip_p = 1; //  IP-protocolnummer 1 voor IPv4 en 58 voor IPv6 icmp. (wikipedia)
    iph->ip_src = _address;
    iph->ip_dst = IPAddress("255.255.255.255");
    iph->ip_sum = click_in_cksum((unsigned char*)packet->data(), packet->length());

    SollicitationPacketheader* sh = (SollicitationPacketheader*) (iph+1);
    sh->type = 10; // Sollicitations
    sh->code = 0; // 0

    packet->set_dst_ip_anno(iph->ip_dst);
    sh->checksum = click_in_cksum((unsigned char *) sh, packet->length()-sizeof(click_ip));

    return packet;
}

// expects node Sollicitation packet
// sends an extra packet, we do not reset the timer
void AgentSolicitation::push(int, Packet *p) {
    Packet *newP = makePacket();
    if(newP)
    {
        output(0).push(newP);
    }
    p->kill();
}


// make packet and increase seq num
void AgentSolicitation::run_timer(Timer * timer) {
    Packet *p = makePacket();
    if (p) {
        output(0).push(p);
        click_chatter("Sollicitation sent");
    }

    // random term to make sure 2 hosts messages will not interfere
    timer->reschedule_after_msec(1000+((rand()%20)-10));
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AgentSolicitation)
