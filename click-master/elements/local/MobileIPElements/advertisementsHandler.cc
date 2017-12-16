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
    AdvertisementPacketheader* advh = (advertisement_header*)(ip + 1);
    //ah->flagsReserved = (_FA << 7) + (_HA << 5) + (_FA << 4) + 0;
    uint16_t flags = advh->flagsReserved;

    // create struct if we need to add
    Advertisement advStruct;
    advStruct.lifetime = advh->lifetime;
    advStruct.reg_lifetime = advh->lifetimeEx;
    advStruct.sequenceNum = advh->sequenceNum;
    advStruct.COA = advh->addressEx;
    advStruct.private_addr = advh->adress;
    advStruct.ha = false;
    advStruct.fa = false;
    if((flags >> 5) & 1){
        advStruct.ha = true;
    }
    if(((flags >> 7) & 1) && ((_FA << 4) & 1)){
        advStruct.fa = true;
    }
    if(advStruct.ha == false && advStruct.fa == false){
        return;
    }
    _mobileNode->advertisementReady = true;
    // modify current adv if needed. also extract the seq num
    // check if the adv is from the curr one if connected
    bool found = false;
    int LastSeq = -1;
    for (Vector<Advertisement>::iterator it = current_advertisements.begin(); it != currentRequests.end(); ++it){
        // if we have an entry in the list. We can update it
        // this happens when we are connected but not necessairly with this agent
        if(it->private_addr == advStruct.private_addr && it->COA == advStruct.COA){
            found = true;
            // if the router is reset and i am connected to that one, re reg with new values
            if(advh.sequenceNum < 256 && advh.sequenceNum <= it->sequenceNum && _mobileNode->curr_private_addr == it->private_addr){
                if(_mobileNode->curr_private_addr == it->private_addr){
                    // make packet (req) for this advert _source
                    // stuff
                }
                // update fields recording to the curr adv message,
                it->lifetime = advh.lifetime;
                it->reg_lifetime = advh.lifetimeEx;
                it->sequenceNum = advh.sequenceNum;
            }
        }
    }
    if(found == false){
        current_advertisements.push_back(advStruct);
    }
    if(_mobileNode->connected == false){
        // make packet (req) for this advert _source
    }
    // is there is a change FA to HA
    else if(_mobileNode->connected == true && advh->adress == _mobileNode->home_private_addr
        && _mobileNode->curr_coa!= _mobileNode->home_public_addr){
            // make packet (req) for this advert _source DEREG
        }
}


// make packet and increase seq num
void AdvertisementsHandler::run_timer(Timer * timer) {
    bool wasConnected = _mobileNode->connected;
    bool hostConnectionLost = false;
    for (Vector<Advertisement>::iterator it = current_advertisements.begin(); it != currentRequests.end();){
        if(it->lifetime == 0){
            // if my host is not active anymore
            if(wasConnected && it->private_addr == _mobileNode->curr_private_addr){
                hostConnectionLost = true;
                _mobileNode->connected = false;
                current_advertisements.erase(it);
            }
        }else{
            it->lifetime--;
            it++;
        }
    }
    if(current_advertisements.empty()){
        _mobileNode->advertisementReady = false
    }
    if((!wasConnected || hostConnectionLost) && !current_advertisements.empty()){
        // make packet req with any item in list
    }

    timer->reschedule_after_msec(1000);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AdvertisementsHandler)
