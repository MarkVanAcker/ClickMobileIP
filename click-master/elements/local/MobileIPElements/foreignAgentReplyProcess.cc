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

    VisitorList* templist;

    if (Args(conf, this, errh).read_mp("FAGENT", _foreignAgent).read_m("VISITOR",
    ElementCastArg("VisitorList"),
    templist).complete() < 0) return -1;

    _visitorList = templist;
    _maxLifetime = 1800; // default value
	return 0;
}

// expects replies without an error code
void ForeignAgentReplyProcess::push(int, Packet *p) {
    // it is assumed that all incoming packets are registration requests
    // get relevant headers
    click_ip *iph = (click_ip*)p->data();
    click_udp *udph = (click_udp*)(iph+1);
RegistrationRequestReplyPacketheader *format = (RegistrationRequestReplyPacketheader*)(udph+1);
    if(format->type != 3) {
        p->kill();
        return;
    }

    // positive respond from home agent
    // update list
    if(format->code == 1){
        click_chatter("accept from homeAgent at processReply");
        for(Vector<listItem>::iterator it = _visitorList->_registrationReq.begin();it != _visitorList->_registrationReq.end(); ++it) {
            // we found a corresponding home agent -> check id's
            if (it->homeAgent == iph->ip_src){
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
                    _visitorList->_visitorMap.push_back(item);
                    _visitorList->_registrationReq.erase(it);

                }
            }
        }
    }else{
        for(Vector<listItem>::iterator it = _visitorList->_registrationReq.begin();it != _visitorList->_registrationReq.end(); ++it) {
            if (it->homeAgent == iph->ip_src){
                if(it->id1 == format->id1 && it->id2 == format->id2){
                    _visitorList->_registrationReq.erase(it);
                }
            }
        }
        click_chatter("deny from homeAgent at processReply");
        // respond to node


    }
    output(0).push(p);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(ForeignAgentReplyProcess)
