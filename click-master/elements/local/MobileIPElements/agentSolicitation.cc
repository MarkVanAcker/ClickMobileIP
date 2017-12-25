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

AgentSolicitation::~AgentSolicitation(){}

int AgentSolicitation::configure(Vector<String> &conf,ErrorHandler *errh) {
    MobileInfoList* tempList;
    if (Args(conf, this, errh)
        .read_m("MNBASE",ElementCastArg("MobileInfoList"),tempList)
        .read_m("MAXR", maxRetransmissions)
        .complete() < 0) return -1;

    // should send Sollicitations
    if(maxRetransmissions < 1){
        return -1;
    }

    mobileNode = tempList;
    Timer *timer = new Timer(this);
	timer->initialize(this);
	timer->schedule_after_msec(1);
	return 0;
}


// create adveritesements
Packet* AgentSolicitation::makePacket() {
    int packetsize = sizeof(click_ip) + sizeof(SolicitationPacketheader);
    int headroom = sizeof(click_ether)+ sizeof(struct EtherCrcHeader);
    WritablePacket* packet = Packet::make(headroom, 0, packetsize, 0);
    if (packet == 0){
        click_chatter("Packet creating failed (agent adveritesement)");
        return 0;
    }
    memset(packet->data(), 0, packet->length());
    click_ip* iph = (click_ip*) packet->data();
    iph->ip_v = 4;
    iph->ip_hl = 5;
    iph->ip_len = htons(packet->length());
    iph->ip_ttl = 1; // TTL must be 1 in Sollicitation
    iph->ip_p = 1; //  IP-protocolnummer 1 voor IPv4 en 58 voor IPv6 icmp. (wikipedia)
    iph->ip_src = mobileNode->myAddress;
    iph->ip_dst = IPAddress("255.255.255.255");
    iph->ip_sum = click_in_cksum((unsigned char*)packet->data(), packet->length());

    SolicitationPacketheader* sh = (SolicitationPacketheader*) (iph+1);
    sh->type = 10; // Sollicitations
    sh->code = 0; // 0

    packet->set_dst_ip_anno(iph->ip_dst);
    sh->checksum = click_in_cksum((unsigned char *) sh, packet->length()-sizeof(click_ip));

    return packet;
}



// make packet and increase seq num
void AgentSolicitation::run_timer(Timer * timer) {
    if( transmissions == maxRetransmissions){
        // dont send if we are at max
        maxRetransmissions = 0;
        timer->schedule_after_msec(3000);
        return;
    }
    transmissions++;
    Packet *p = makePacket();
    if (p){
        output(0).push(p);
        click_chatter("Sollicitation sent");
    }
    // random term to make sure 2 hosts messages will not interfere
    timer->reschedule_after_msec(1000+((rand()%20)-10));
    return;
}

// no functionality so far
void AgentSolicitation::push(int, Packet *p) {
        output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AgentSolicitation)
