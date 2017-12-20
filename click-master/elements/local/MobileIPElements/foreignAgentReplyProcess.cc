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

// expects replies without an error code
void ForeignAgentReplyProcess::push(int, Packet *p) {
    //  assume that all incoming packets are registration requests
    // get access packet
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
    if(format->code == 1){
        click_chatter("accept from homeAgent at processReply");
        for(Vector<listItem>::iterator it = _visitorList->_registrationReq.begin();it != _visitorList->_registrationReq.end(); ++it) {
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
                item.lifetimeReq = format->lifetime;
                item.lifetimeRem = format->lifetime; // remaining lifetime
                _visitorList->_registrationReq.erase(it);
                // if there are multiple req sent and multiple replies, update current entry
                for(Vector<listItem>::iterator it = _visitorList->_visitorMap.begin();it != _visitorList->_visitorMap.end();) {
                        if(it->ipSrc == item.ipSrc){
                            _visitorList->_visitorMap.erase(it);
                        }else{
                            it++;
                        }
                }
                _visitorList->_visitorMap.push_back(item);
                break;
            }
        }
    }else{
        click_chatter("deny from homeAgent at processReply");
        for(Vector<listItem>::iterator it = _visitorList->_registrationReq.begin();it != _visitorList->_registrationReq.end(); ++it) {
            if(it->id1 == format->id1 && it->id2 == format->id2){
                _visitorList->_registrationReq.erase(it);
                break;
            }
        }
    }
    iph->ip_sum = htons(0);
    iph->ip_src = _visitorList->_private_addr;
    iph->ip_dst = format->homeAddr;
    iph->ip_sum = click_in_cksum((unsigned char*)iph, sizeof(click_ip));
     // respond to node
    output(0).push(q);
}

void ForeignAgentReplyProcess::run_timer(Timer* timer){
    for(Vector<listItem>::iterator it = _visitorList->_visitorMap.begin();it != _visitorList->_visitorMap.end();) {
        if(it->lifetimeRem == 0){
            _visitorList->_visitorMap.erase(it);
        }else{
            it->lifetimeRem--;
            it++;
        }
    }
    timer->schedule_after_msec(1000);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(ForeignAgentReplyProcess)
