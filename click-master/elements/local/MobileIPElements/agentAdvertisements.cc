#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "agentAdvertisements.hh"

CLICK_DECLS
AgentAdvertiser::AgentAdvertiser():
_sequenceNum(0){
    srand(time(NULL));
}

AgentAdvertiser::~AgentAdvertiser() {}

int AgentAdvertiser::configure(Vector<String> &conf,ErrorHandler *errh) {
    if (Args(conf, this, errh).read_m("FA", _FA)
        .read_m("HA", _HA)
        .read_m("LTREG", _lifetimeReg)
        .read_m("LTADV", _lifetimeAdv)
        .read_m("INTERVAL", _interval)
        .read_m("ADDAGENT", _address)
        .read_m("COA", _addressCO).complete() < 0) return -1;

    // either home or foreign agent
    if(_HA == _FA){
        return -1;
    }
    if(_interval*3 > _lifetimeAdv*1000){
        return -1;
    }

    Timer *timer = new Timer(this);
	timer->initialize(this);
	timer->schedule_after_msec(500);
	return 0;
}


// create adveritesements
Packet* AgentAdvertiser::makePacket() {
    int packetsize = sizeof(click_ip) + sizeof(AdvertisementPacketheader);
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
    iph->ip_id = htons(_sequenceNum);
    iph->ip_ttl = 1; // TTL must be 1 in advertisement
    iph->ip_p = 1; //  IP-protocolnummer 1 voor IPv4 en 58 voor IPv6. (wikipedia)
    iph->ip_src = _address;
    iph->ip_dst = IPAddress("255.255.255.255");
    iph->ip_sum = click_in_cksum((unsigned char*)packet->data(), packet->length());

    AdvertisementPacketheader* ah = (AdvertisementPacketheader*) (iph+1);
    ah->type = 9; // adveritesements
    ah->code = 0; // 0
    ah->lifetime = htons(_lifetimeAdv);
    ah->addressNumbers = 1; // 1 adress
    ah->addrEntrySize = 2; // 2 entries for 1 address
    ah->address = _address; // agent adress
    ah->pref = 0;    // 0
    ah->typeEx = 16; // normal routing
    ah->length = 10; // 6 + 4 bytes
    ah->sequenceNum = htons(_sequenceNum); // next seq
    ah->lifetimeEx = htons(_lifetimeReg);
    // we don not need to use htons() because last bits 0 anyway
    ah->flagsReserved = (_FA << 7) + (_HA << 5) + (_FA << 4) + 0;   // force reg req if FA
    ah->addressEx = _addressCO; // normally same as _address

    packet->set_dst_ip_anno(iph->ip_dst);
    ah->checksum = click_in_cksum((unsigned char *) ah, packet->length()-sizeof(click_ip));

    return packet;
}

// expects node Sollicitation packet
// sends an extra packet, we do not reset the timer
void AgentAdvertiser::push(int, Packet *p) {
    Packet *newP = makePacket();
    if(newP)
    {
        output(0).push(newP);
    }
    p->kill()
}


// make packet and increase seq num
void AgentAdvertiser::run_timer(Timer * timer) {
    Packet *p = makePacket();
    if (p) {
        output(0).push(p);
        if (_sequenceNum == 0xffff)
            _sequenceNum = 256;
        else
            _sequenceNum++;

        click_chatter("Advertisement sent");
    }

    // random term to make sure 2 hosts messages will not interfere
    timer->reschedule_after_msec(_interval+((rand()%70)-35));
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AgentAdvertiser)
