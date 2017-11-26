#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "ipEncapsulation.hh"

CLICK_DECLS
IpEncapsulation::IpEncapsulation()
{}

IpEncapsulation::~IpEncapsulation()
{}

int IpEncapsulation::configure(Vector<String> &conf, ErrorHandler *errh) {
    bindingsList* templist;
    if (Args(conf, this, errh).read_mp("IPADDRES", _tunnelAddres).read("BINDING",
    ElementCastArg("bindingsList"),
    templist).complete() < 0) return -1;

    _bindingsList = templist;
	return 0;
}

/* There should always be an adress found here because the packet is forwarded to the encap */
// depends on how we want to use the bingslist.
IPAddress IpEncapsulation::getDesitination(IPAddress MN){
    return _bindingsList->_table.find_pair(MN)->value->mobile_node_coa;
}


void IpEncapsulation::push(int, Packet *p) {
    click_chatter("Packet creating failed0 (IPENCAP)");
    // get the careoff adress of the bindingslist
    click_ip* iph = (click_ip*)p->data();
    IPAddress careOff = getDesitination(iph->ip_dst);
    // new packet with an extra ip header, create headroom to push upon
    // space is not the same as packetdata
    click_chatter("Packet creating failed00000 (IPENCAP)");

    int packetsize = p->length();
    int headroom =  sizeof(click_ether) + sizeof(click_ip);
    WritablePacket *packet = Packet::make(headroom, 0, packetsize, 0);
    if (packet == 0)
    {
        click_chatter("Packet creating failed (IPENCAP)");
        return;
    }
    click_chatter("111111111 (IPENCAP)");

    // copy old packet in the new packet's data
    memcpy(packet->data(), p->data(), p->length());

    // create space for header
    click_chatter("Pa222222222222(IPENCAP)");

    packet = packet->push(sizeof(click_ip));
    click_chatter("Pa33333333333(IPENCAP)");

    // add pushed header at the start of the packet
    memset(packet->data(), 0, sizeof(click_ip));
    click_chatter("Packet creating failed1222 (IPENCAP)");

    click_ip* Oiph = (click_ip*)packet->data(); // outer ip
    click_chatter("Packet creating failed4444(IPENCAP)");

    Oiph->ip_v = 4;
    Oiph->ip_hl = sizeof(click_ip) >> 2;
    Oiph->ip_len = htons(packet->length());
    Oiph->ip_id = htons(1);
    Oiph->ip_ttl = 200;
    click_chatter("Packet creating failed1222 (IPENCAP)");

    Oiph->ip_p = 4; // ip in ip protocol
    Oiph->ip_src = _tunnelAddres;
    click_chatter("Packet creating failedtest (IPENCAP)");


    Oiph->ip_dst = careOff; //end of tunnel
    click_chatter("Packet creating failedtest222 (IPENCAP)");

    Oiph->ip_sum = click_in_cksum((unsigned char*)Oiph, sizeof(click_ip));
    p->kill(); // free memory
    packet->set_dst_ip_anno(iph->ip_dst);
    click_chatter("Packet creating failed3333 (IPENCAP)");


    output(0).push(packet);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(IpEncapsulation)
