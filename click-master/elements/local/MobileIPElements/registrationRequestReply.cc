#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>

#include "registrationRequestReply.hh"
#include "registrationRequestSource.hh"

CLICK_DECLS
RegistrationRequestReply::RegistrationRequestReply()
{}

RegistrationRequestReply::~RegistrationRequestReply()
{}

int RegistrationRequestReply::configure(Vector<String> &conf, ErrorHandler *errh) {
    AgentBase* templist;
    if (Args(conf, this, errh).read("BINDING", ElementCastArg("AgentBase"), templist).complete() < 0) return -1;

    bindingsList = templist;
	timer = new Timer(this);
	timer->initialize(this);
	timer->reschedule_after_msec(1000);
	return 0;
}

/* return 0 if accepted, return 1 for accepted but simultanious bindings unsupported.
    return error corresponding with the reason for deny */

// WHAT IF IM CONNECTED ALLREADY ?
unsigned short int RegistrationRequestReply::validatePacket(Packet *p){
    // get all package dimensions
    click_ip *iph = (click_ip*)p->data();
    click_udp *udph = (click_udp*)(iph+1);
    RegistrationRequestPacketheader *format = (RegistrationRequestPacketheader*)(udph+1);

    // 136 ???

    // sent as zero, ignore
    uint8_t flags = format->flags;
    if((flags & 1) || (flags >> 2) & 1) { // logical AND with a shift right logical
        return 134;
    }

    // deny co-located Care-of-Adress (Node will only be at home or at forein host)
    if((flags >> 5) & 1) {
        return 128;
    }

    // not the same public address
    // in dereg message only
    if(format->homeAgent != bindingsList->public_addr){
        return 136;
    }
    //accepted but simultanious bindings unsupported
    // !!!!SHOULD BE 1 BUT DOES NOT WORK IN VM!!!!
    return 0;

}


void RegistrationRequestReply::push(int, Packet *p) {
    // it is assumed that all incoming packets are registration requests
    // get relevant headers
    click_ip *iph = (click_ip*)p->data();
    click_udp *udph = (click_udp*)(iph+1);
    RegistrationRequestPacketheader *format = (RegistrationRequestPacketheader*)(udph+1);

    if(format->type != 1) {
        // wrong package type
        p->kill();
        return;
    }
    // validate packet content, react accordingly
    unsigned short int code = validatePacket(p);

    bool duplicate = false;
    uint16_t lifetimeDup = 0;
    // if a binding exists (if node not at home)
    if(!bindingsList->isHome(format->homeAddr)){
        RegistrationTable::Pair * pair = bindingsList->table.find_pair(format->homeAddr);
        if(pair->value->id1 == format->id1 && pair->value->id2 == format->id2 &&
          pair->value->mobile_node_homadress == format->homeAddr && pair->value->mobile_node_coa == format->coAddr){
            duplicate = true;
            lifetimeDup = pair->value->lifetime;
        }
    }


    // respond to node
    int packetSize = sizeof(struct RegistrationRequestReplyPacketheader) + sizeof(click_ip) + sizeof(click_udp);
    int headroom = sizeof(click_ether)+ sizeof(struct EtherCrcHeader);
    WritablePacket *packet = Packet::make(headroom, 0, packetSize, 0);
    if(packet == 0) {
        click_chatter("Could not make packet");
        return;
    }
    memset(packet->data(), 0, packet->length());

    click_ip *iphNew = (click_ip *)packet->data();
    iphNew->ip_v = 4;
    iphNew->ip_hl = sizeof(click_ip) >> 2;
    iphNew->ip_len = htons(packet->length());
    iphNew->ip_id = htons(1);
    iphNew->ip_p = 17;
    iphNew->ip_ttl = 64;
    iphNew->ip_src = iph->ip_dst;
    iphNew->ip_dst = iph->ip_src;
    iphNew->ip_sum = click_in_cksum((unsigned char *)iphNew, sizeof(click_ip));

    packet->set_dst_ip_anno(iphNew->ip_dst); 

    click_udp *udphNew = (click_udp*)(iphNew+1);
    udphNew->uh_sport = udph->uh_dport;
    udphNew->uh_dport = udph->uh_sport;
    udphNew->uh_ulen = htons(packet->length()-sizeof(click_ip));

    RegistrationRequestReplyPacketheader *formatNew = (RegistrationRequestReplyPacketheader*)(udphNew+1);
    formatNew->type = 3; // Registration Reply
    formatNew->code = code;
    if (duplicate){
        formatNew->lifetime = lifetimeDup;
    }else{
        formatNew->lifetime = format->lifetime;
    }
    formatNew->homeAddr = format->homeAddr;
    formatNew->homeAgent = format->homeAgent;
    formatNew->id1 = format->id1;
    formatNew->id2 = format->id2;

    // Calculate the udp checksum
    udphNew->uh_sum = click_in_cksum_pseudohdr(click_in_cksum((unsigned char*)udphNew, packetSize - sizeof(click_ip)),
    iphNew, packetSize - sizeof(click_ip));


    // if accepted change bindings
    if (code == 0 || code == 1)
    {
		if(format->lifetime > 0){
		    // remove node from a previous foreign binding in a list/map!
		    // if home netwerk is the same as CoA remove binding!

		    //update bindlist
		    HARegistrationEntry * tempentry = new HARegistrationEntry;
		    tempentry->mobile_node_coa = format->coAddr;
		    tempentry->lifetime = format->lifetime;
		    tempentry->id1 = format->id1;
		    tempentry->id2 = format->id2;
		    tempentry->mobile_node_homadress = format->homeAddr;

		    bindingsList->table.insert(format->homeAddr,tempentry);
            bool found = false;
            for(RegistrationIPList::iterator it = bindingsList->list.begin(); it != bindingsList->list.end();it++){
                if ((*it) == format->homeAddr){
                    found = true;
                    break;
                }
            }
            if(!found){
                // add an IP in de list if it does not exist
                bindingsList->list.push_back(format->homeAddr);
            }
		}else{
			if(!bindingsList->isHome(format->homeAddr)){
				bindingsList->table.erase(format->homeAddr);
				for(RegistrationIPList::iterator it = bindingsList->list.begin(); it != bindingsList->list.end();){
					if((*it) == format->homeAddr){
						bindingsList->list.erase(it);
						break;
					}else{
						it++;
					}
				}

			}

		}

        // keep info is the current situation (new bind) in a list or map!
		if(format->lifetime > 0){
		output(0).push(packet);
		}else{
		output(1).push(packet);
		}
    }



}

void RegistrationRequestReply::run_timer(Timer * timer) {

	for(RegistrationIPList::iterator it = bindingsList->list.begin(); it != bindingsList->list.end();){
		RegistrationTable::Pair * pair = bindingsList->table.find_pair((*it));
		pair->value->lifetime = pair->value->lifetime-htons(1);
		if(pair->value->lifetime == 0){
			bindingsList->table.erase((*it));
			bindingsList->list.erase(it);
		}else{
			it++;
		}
	}

    timer->reschedule_after_msec(1000);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RegistrationRequestReply)
