#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>


CLICK_DECLS
VisitorList::VisitorList(){}

VisitorList::~VisitorList(){}

int VisitorList::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).read_mp("ADD", _homeAddress).complete() < 0) return -1;

    _maxRequests = 5;
	Timer *timer = new Timer(this);
	timer->initialize(this);
	timer->schedule_after_msec(1000);
	return 0;
}

void VisitorList::run_timer(Timer *timer){

    for(Vector<listItem>::iterator it = _registrationReq.begin();it != _registrationReq.end(); ++it) {
        uint16_t val = *it.lifetimeRem -1;
        if(val == 0){
            _registrationReq.erase(it);
        }else{
                *it.lifetimeRem = val;
        }
    }
    for(Vector<listItem>::iterator it = _visitorMap.begin();it != _visitorMap.end(); ++it) {
        uint16_t val = *it.lifetimeRem -1;
        if(val == 0){
            _visitorMap.erase(it);
        }else{
                *it.lifetimeRem = val;
        }
    }
    timer->reschedule_after_msec(1000);
}

bool VisitorList::inMapHome(IPAddress home){
    for(Vector<listItem>::iterator it = _visitorMap.begin();it != _visitorMap.end(); ++it) {
        if (*it.homeAgent == node){
            return true;
        }
    }
    return false;
}

bool VisitorList::inPendingHome(IPAddress home){
    for(Vector<listItem>::iterator it = _registrationReq.begin();it != _registrationReq.end(); ++it) {
        if (*it.homeAgent == node){
            return true;
        }
    }
    return false;
}

bool VisitorList::inMapNode(IPAddress node){
    for(Vector<listItem>::iterator it = _visitorMap.begin();it != _visitorMap.end(); ++it) {
        if (*it.ipSrc == node){
            return true;
        }
    }
    return false;
}

bool VisitorList::inPendingNode(IPAddress node){
    for(Vector<listItem>::iterator it = _registrationReq.begin();it != _registrationReq.end(); ++it) {
        if (*it.ipSrc == node){
            return true;
        }
    }
    return false;
}

void VisitorList::push(int, Packet *p){

}



CLICK_ENDDECLS
EXPORT_ELEMENT(VisitorList)
