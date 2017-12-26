#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "agentAdvertisements.hh"
#include <iostream>

CLICK_DECLS
AgentAdvertiser::AgentAdvertiser():
sequenceNum(0){
    srand(time(NULL));
}

AgentAdvertiser::~AgentAdvertiser() {}

int AgentAdvertiser::configure(Vector<String> &conf,ErrorHandler *errh) {
    AgentBase* templist;
    if (Args(conf, this, errh).read_m("FA", FA)
        .read_m("HA", HA)
        .read_m("LTADV", lifetimeAdv)
        .read_m("INTERVAL", interval)
        .read_m("BASE",ElementCastArg("AgentBase"),
        templist).complete() < 0) return -1;

    if(interval*3 > lifetimeAdv){
        click_chatter("Interval is greater than 1/3 lifetime of the adv");
        return -1;
    }

    if(!HA && !FA){
        click_chatter("Agent must be either (or both) FA or HA");
        return -1;
    }


    agent = templist;
    sequenceNum = htons(0);
	timer = new Timer(this);
	timer->initialize(this);
	timer->schedule_after_msec(1);
	return 0;
}


// create adveritesements
Packet* AgentAdvertiser::createPacket() {
    int packetSize =  sizeof(click_ip) + sizeof(AdvertisementPacketheader);
    int headroom = sizeof(click_ether)+ sizeof(struct EtherCrcHeader);
    WritablePacket* packet = Packet::make(headroom, 0, packetSize, 0);
    if (packet == 0){
        click_chatter("Packet creating failed (agent adveritesement)");
        return 0;
    }
    memset(packet->data(), 0, packet->length());
    click_ip* iph = (click_ip*) packet->data();
    iph->ip_v = 4;
    iph->ip_hl = sizeof(click_ip) >> 2;
    iph->ip_len = htons(packet->length());
    iph->ip_id = htons(sequenceNum);
    iph->ip_ttl = 1; // TTL must be 1 in advertisement
    iph->ip_p = 1; //  IP-protocolnummer 1 voor IPv4 en 58 voor IPv6. (wikipedia)
    iph->ip_src = agent->private_addr;
    iph->ip_dst = IPAddress("255.255.255.255");
    iph->ip_sum = click_in_cksum((unsigned char*)packet->data(), packet->length());

    AdvertisementPacketheader* ah = (AdvertisementPacketheader*) (iph+1);
    ah->type = 9; // adveritesements
    ah->code = 0; // 0
    ah->lifetime = htons(lifetimeAdv);
    ah->addressNumbers = 1; // 1 adress
    ah->addrEntrySize = 2; // 2 entries for 1 address
    ah->address = agent->public_addr; // agent adress
    ah->pref = 0;    // 0
    ah->typeEx = 16; // normal routing
    ah->length = 10; // 6 + 4 bytes
    ah->sequenceNum = sequenceNum; // next seq
    ah->lifetimeEx = htons(agent->lifetimeReg);
    // we don not need to use htons() because last bits 0 anyway
    ah->flagsReserved = (FA << 7) + (HA << 5) + (FA << 4) + 0;   // force reg req if FA
    ah->addressEx = agent->public_addr; // coa

    packet->set_dst_ip_anno(iph->ip_dst);
    ah->checksum = click_in_cksum((unsigned char *) ah, packet->length()-sizeof(click_ip));
    if (sequenceNum == 0xffff){
        sequenceNum = htons(256);
    }else{
        sequenceNum = sequenceNum+htons(1);
    }

    return packet;
}

// expects node Sollicitation packet
// sends an extra packet, we do not reset the timer
void AgentAdvertiser::push(int, Packet *p) {
    Packet *newP = createPacket();
    if(newP)
    {
        output(0).push(newP);
    }
    p->kill();
    timer->reschedule_after_msec(interval+((rand()%70)-35));
}


// make packet and increase seq num
void AgentAdvertiser::run_timer(Timer * timer) {
    Packet *p = createPacket();
    if (p) {
        output(0).push(p);
    }

    // random term to make sure 2 hosts messages will not interfere
    timer->reschedule_after_msec(interval+((rand()%70)-35));
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AgentAdvertiser)
