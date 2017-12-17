#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "advertisementsHandler.hh"

CLICK_DECLS
AdvertisementsHandler::AdvertisementsHandler(){
    srand(time(NULL));
}

AdvertisementsHandler::~AdvertisementsHandler() {}

// reg source as argument
int AdvertisementsHandler::configure(Vector<String> &conf,ErrorHandler *errh) {
    MobileInfoList* tempList;
    RegistrationRequestSource* tempSource;
    if (Args(conf, this, errh).read_m("MNLIST",ElementCastArg("MobileInfoList"),tempList)
                              .read_m("SOURCE",ElementCastArg("registrationRequestSource"),tempSource)
                              .complete() < 0) return -1;

    _source = tempSource;
    _mobileNode = tempList;
    Timer *timer = new Timer(this);
    timer->initialize(this);
    timer->schedule_after_msec(1000);
    return 0;
}


// recieves Advertisements yet to be handled
void AdvertisementsHandler::push(int, Packet *p) {
    click_ip* ip = (click_ip*)p->data();
    AdvertisementPacketheader* advh = (AdvertisementPacketheader*)(ip + 1);
    uint16_t flags = advh->flagsReserved;
    // create struct if we need to add
    Advertisement advStruct;
    advStruct.lifetime = advh->lifetime;
    advStruct.reg_lifetime = advh->lifetimeEx;
    advStruct.sequenceNum = advh->sequenceNum;
    advStruct.COA = advh->addressEx;
    advStruct.private_addr = advh->address;
    advStruct.ha = false;
    advStruct.fa = false;
    if((flags >> 5) & 1){
        advStruct.ha = true;
    }
    if(((flags >> 7) & 1) && ((flags << 4) & 1)){
        advStruct.fa = true;
    }
    if(advStruct.ha == false && advStruct.fa == false){
        return;
    }
    _mobileNode->advertisementReady = true;
    // modify current adv if needed. also extract the seq num
    // check if the adv is from the curr one if connected
    bool found = false;
    for (Vector<Advertisement>::iterator it = _mobileNode->current_advertisements.begin(); it != _mobileNode->current_advertisements.end(); ++it){
        // if we have an entry in the list. We can update it
        // this happens when we are connected but not necessairly with this agent
        if(it->private_addr == advStruct.private_addr && it->COA == advStruct.COA){
            found = true;
            // if the router is reset and i am connected to that one, re reg with new values
            if(advh->sequenceNum < 256 && advh->sequenceNum <= it->sequenceNum && _mobileNode->curr_private_addr == it->private_addr){
                if(_mobileNode->curr_private_addr == it->private_addr){
                    _source->makePacket(advStruct);
                }
                // update fields recording to the curr adv message,
                it->lifetime = advh->lifetime;
                it->reg_lifetime = advh->lifetimeEx;
                it->sequenceNum = advh->sequenceNum;
            }
        }
    }
    if(found == false){
        _mobileNode->current_advertisements.push_back(advStruct);
    }
    if(_mobileNode->connected == false){
        _source->makePacket(advStruct);
        return;
    }
    // is there is a change FA to HA
    else if(_mobileNode->connected == true && advh->address == _mobileNode->home_private_addr
        && _mobileNode->curr_coa!= _mobileNode->home_public_addr){
            _source->makePacket(advStruct);
        }
}


// decrease lifetimes and act if needed
void AdvertisementsHandler::run_timer(Timer * timer) {
    bool wasConnected = _mobileNode->connected;
    bool hostConnectionLost = false;
    for (Vector<Advertisement>::iterator it = _mobileNode->current_advertisements.begin(); it != _mobileNode->current_advertisements.end();){
        if(it->lifetime == 0){
            // if my host is not active anymore
            if(wasConnected && it->private_addr == _mobileNode->curr_private_addr){
                hostConnectionLost = true;
                _mobileNode->connected = false;
                _mobileNode->current_advertisements.erase(it);
            }
        }else{
            it->lifetime--;
            it++;
        }
    }
    if(_mobileNode->current_advertisements.empty()){
        _mobileNode->advertisementReady = false;
    }
    if((!wasConnected || hostConnectionLost) && !_mobileNode->current_advertisements.empty()){
        _source->makePacket(*_mobileNode->current_advertisements.begin());
    }

    timer->reschedule_after_msec(1000);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AdvertisementsHandler)
