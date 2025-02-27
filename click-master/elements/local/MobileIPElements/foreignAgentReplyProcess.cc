#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>

#include "foreignAgentReplyProcess.hh"
#include "registrationRequestReply.hh"

CLICK_DECLS
ForeignAgentReplyProcess::ForeignAgentReplyProcess()
{}

ForeignAgentReplyProcess::~ForeignAgentReplyProcess()
{}

int ForeignAgentReplyProcess::configure(Vector<String> &conf, ErrorHandler *errh) {

    AgentBase* templist;

    if (Args(conf, this, errh).read_m("AGBASE",
    ElementCastArg("AgentBase"),
    templist).complete() < 0) return -1;

    visitorList = templist;
	return 0;
}

// expects replies without an error code
void ForeignAgentReplyProcess::push(int, Packet *p) {
    //  assume that all incoming packets are registration requests
    // get access packet
    unsigned int packetsize = p->length();
    if(packetsize < (sizeof(click_ip) + sizeof(click_udp) + sizeof(RegistrationRequestReplyPacketheader))){
        p->kill();
        return;
    }
    WritablePacket* q = p->uniqueify();
    click_ip *iph = (click_ip*)q->data();
    click_udp *udph = (click_udp*)(iph+1);
    RegistrationRequestReplyPacketheader *format = (RegistrationRequestReplyPacketheader*)(udph+1);
    if(format->type != 3) {
        q->kill();
        return;
    }

    // positive respond from home agent
    // update list
    if(format->code == 0 || format->code == 1){
        click_chatter("accept from homeAgent at processReply");
        for(Vector<listItem>::iterator it = visitorList->registrationReq.begin();it != visitorList->registrationReq.end(); it++) {
            // we found a corresponding home agent -> check id's
            if(it->id1 == format->id1 && it->id2 == format->id2){
                listItem item;
                item.ethSrc =  it->ethSrc;
                item.ipSrc = it->ipSrc;
                item.ipDst = it->ipDst;
                item.udpSrc = it->udpSrc;
                item.homeAgent = it->homeAgent;
                item.id1 = it->id1;
                item.id2 = it->id2;
                if (it->lifetimeReq < format->lifetime){
                    // must take the MIN of the request and reply
                    item.lifetimeReq = it->lifetimeReq;
                    item.lifetimeRem = it->lifetimeReq; // remaining lifetime
                }else{
                    // could be equal of less than the request (depends on HA)
                    item.lifetimeReq = format->lifetime;
                    item.lifetimeRem = format->lifetime; // remaining lifetime
                }
                visitorList->registrationReq.erase(it);
                // if there are multiple req sent and multiple replies, update current entry
                for(Vector<listItem>::iterator it = visitorList->visitorMap.begin();it != visitorList->visitorMap.end();) {
                        if(it->ipSrc == item.ipSrc){
                            visitorList->visitorMap.erase(it);
                        }else{
                            it++;
                        }
                }
                visitorList->visitorMap.push_back(item);
                break;
            }
        }
    }else{
        // no need to take the MIN of both lifetimes (req/reply) because it is denied anyway
        click_chatter("deny from homeAgent at processReply");
        for(Vector<listItem>::iterator it = visitorList->registrationReq.begin();it != visitorList->registrationReq.end(); it++) {
            if(it->id1 == format->id1 && it->id2 == format->id2){
                visitorList->registrationReq.erase(it);
                break;
            }
        }
    }
    int packetSize = sizeof(struct RegistrationRequestReplyPacketheader) + sizeof(click_ip) + sizeof(click_udp);
    iph->ip_sum = htons(0);
    iph->ip_src = visitorList->private_addr;
    iph->ip_dst = format->homeAddr;
    iph->ip_sum = click_in_cksum((unsigned char*)iph, sizeof(click_ip));
    udph->uh_sum = htons(0);
    udph->uh_sum = click_in_cksum_pseudohdr(click_in_cksum((unsigned char*)udph, packetSize - sizeof(click_ip)),
    iph, packetSize - sizeof(click_ip));
	q->set_dst_ip_anno(format->homeAddr);
     // respond to node
    output(0).push(q);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(ForeignAgentReplyProcess)
