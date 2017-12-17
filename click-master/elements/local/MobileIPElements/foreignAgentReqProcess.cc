#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>

#include "foreignAgentReqProcess.hh"
#include "registrationRequestSource.hh"

CLICK_DECLS
ForeignAgentReqProcess::ForeignAgentReqProcess()
{}

ForeignAgentReqProcess::~ForeignAgentReqProcess()
{}

int ForeignAgentReqProcess::configure(Vector<String> &conf, ErrorHandler *errh) {

    VisitorList* templist;

    if (Args(conf, this, errh).read_mp("FAGENT", _foreignAgent).read_m("VISITOR",
    ElementCastArg("VisitorList"),
    templist).complete() < 0) return -1;

    Timer *timer = new Timer(this);
    timer->initialize(this);
    timer->schedule_after_msec(1000);
    _visitorList = templist;
    _maxLifetime = 1800; // default value
	return 0;
}

/* forward the request if it is well formed, if not return an errorcode */
unsigned short int ForeignAgentReqProcess::validatePacket(Packet *p){
    // get all package dimensions
    click_ip *iph = (click_ip*)p->data();
    click_udp *udph = (click_udp*)(iph+1);
    RegistrationRequestPacketheader *format = (RegistrationRequestPacketheader*)(udph+1);

    // requested lifetime too long
    uint16_t lifetime = format->lifetime;
    uint8_t flags = format->flags;
    if(ntohs(lifetime) > _maxLifetime) {
        click_chatter("Deny because of lifetime");
        return 69;
    }

    // sent as 0
    if((flags) & 1 || (flags >> 2) & 1) {
        click_chatter("sent as zero");
        return 70;
    }

    if(_visitorList->_registrationReq.size() == _visitorList->_maxRequests){
        click_chatter("too much");
        return 66;
    }

    // create VisitorList pending entry
    listItem item;
    item.ipSrc = format->homeAddr;
    item.ipDst = iph->ip_dst;
    item.udpSrc = udph->uh_sport;
    item.udpDst = udph->uh_dport;
    item.homeAgent = format->homeAgent;
    item.id1 = format->id1;
    item.id2 = format->id2;
    item.lifetimeReq = ntohs(format->lifetime);
    item.lifetimeRem = ntohs(format->lifetime);

    // should be true
    if(_visitorList->_registrationReq.size() < _visitorList->_maxRequests){
        _visitorList->_registrationReq.push_back(item);
    }

    return 1;
}


void ForeignAgentReqProcess::push(int, Packet *p) {
    // it is assumed that all incoming packets are registration requests
    // get relevant headers
    click_ip *iph = (click_ip*)p->data();
    click_udp *udph = (click_udp*)(iph+1);
    RegistrationRequestPacketheader *format = (RegistrationRequestPacketheader*)(udph+1);

    if(format->type != 1) {
        p->kill();
        return;
    }

    // validate packet content, react accordingly
    unsigned short int code = validatePacket(p);
    if(code == 1){
        output(1).push(p);
        return;
    }else{
        click_chatter("fault in packet recieved (PROCESS REQUEST)");
        // respond to node
        int packet_size = sizeof(struct ForeignAgentReqProcessPacketheader) + sizeof(click_ip) + sizeof(click_udp);
        int headroom = sizeof(click_ether);
        WritablePacket *packet = Packet::make(headroom, 0, packet_size, 0);
        if(packet == 0) {
            click_chatter("Could not make packet");
            return;
        }
        memset(packet->data(), 0, packet->length());

        click_ip *iphNew = (click_ip *)packet->data();
        iphNew->ip_v = 4;
        iphNew->ip_p = 17;
        iphNew->ip_hl = sizeof(click_ip) >> 2;
        iphNew->ip_len = htons(packet->length());
        iphNew->ip_id = htons(1);
        iphNew->ip_ttl = 12;
        iphNew->ip_src = iph->ip_dst;
        iphNew->ip_dst = iph->ip_src;
        iphNew->ip_sum = click_in_cksum((unsigned char *)iphNew, sizeof(click_ip));

        packet->set_dst_ip_anno(iphNew->ip_dst); //not sure why it is used

        click_udp *udphNew = (click_udp*)(iphNew+1);
        udphNew->uh_sport = udph->uh_dport;
        udphNew->uh_dport = udph->uh_sport;
        udphNew->uh_ulen = htons(packet->length()-sizeof(click_ip));

        ForeignAgentReqProcessPacketheader *formatNew = (ForeignAgentReqProcessPacketheader*)(udphNew+1);
        formatNew->type = 3; // Registration Reply
        formatNew->code = 78;
        formatNew->lifetime = format->lifetime;
        formatNew->homeAddr = format->homeAddr;
        formatNew->homeAgent = format->homeAgent;
        formatNew->id1 = format->id1;
        formatNew->id2 = format->id2;

        // Calculate the udp checksum
        udphNew->uh_sum = click_in_cksum_pseudohdr(click_in_cksum((unsigned char*)udphNew, packet_size - sizeof(click_ip)),
        iphNew, packet_size - sizeof(click_ip));

        output(0).push(packet);
    }

}

void ForeignAgentReqProcess::run_timer(Timer* timer){
    // edit pending registration
    for(Vector<listItem>::iterator it = _visitorList->_registrationReq.begin(); it != _visitorList->_registrationReq.end();){
        if(it->lifetimeRem == 0){
            // expired
            _visitorList->_registrationReq.erase(it);
        }else{
            if(it->lifetimeReq - it->lifetimeRem == 7){
                _visitorList->_registrationReq.erase(it);
                int packet_size = sizeof(struct ForeignAgentReqProcessPacketheader) + sizeof(click_ip) + sizeof(click_udp);
                int headroom = sizeof(click_ether);
                WritablePacket *packet = Packet::make(headroom, 0, packet_size, 0);
                if(packet == 0) {
                    click_chatter("Could not make packet");
                    return;
                }
                memset(packet->data(), 0, packet->length());

                click_ip *iphNew = (click_ip *)packet->data();
                iphNew->ip_v = 4;
                iphNew->ip_p = 17;
                iphNew->ip_hl = sizeof(click_ip) >> 2;
                iphNew->ip_len = htons(packet->length());
                iphNew->ip_id = htons(1);
                iphNew->ip_ttl = 12;
                iphNew->ip_src = it->ipDst;
                iphNew->ip_dst = it->ipSrc;
                iphNew->ip_sum = click_in_cksum((unsigned char *)iphNew, sizeof(click_ip));

                packet->set_dst_ip_anno(iphNew->ip_dst); //not sure why it is used

                click_udp *udphNew = (click_udp*)(iphNew+1);
                udphNew->uh_sport = it->udpDst;
                udphNew->uh_dport = it->udpSrc;
                udphNew->uh_ulen = htons(packet->length()-sizeof(click_ip));

                ForeignAgentReqProcessPacketheader *formatNew = (ForeignAgentReqProcessPacketheader*)(udphNew+1);
                formatNew->type = 3; // Registration Reply
                formatNew->code = 78;
                formatNew->lifetime = htons(it->lifetimeReq);
                formatNew->homeAddr = it->ipSrc;
                formatNew->homeAgent = it->homeAgent;
                formatNew->id1 = it->id1;
                formatNew->id2 = it->id2;

                // Calculate the udp checksum
                udphNew->uh_sum = click_in_cksum_pseudohdr(click_in_cksum((unsigned char*)udphNew, packet_size - sizeof(click_ip)),
                iphNew, packet_size - sizeof(click_ip));

                output(0).push(packet);
            }else{
                it->lifetimeRem--;
                it++;
            }
        }
    }
    timer->schedule_after_msec(1000);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(ForeignAgentReqProcess)
