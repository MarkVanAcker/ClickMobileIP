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
    if (Args(conf, this, errh).read_m("MNBASE",ElementCastArg("MobileInfoList"),tempList)
                              .read_m("SOURCE",ElementCastArg("RegistrationRequestSource"),tempSource)
                              .complete() < 0) return -1;

    source = tempSource;
    mobileNode = tempList;
    Timer *timer = new Timer(this);
    timer->initialize(this);
    timer->schedule_after_msec(1000);
    return 0;
}


// recieves Advertisements yet to be handled
void AdvertisementsHandler::push(int, Packet *p) {
    click_chatter("RECIEVED ADV");
    click_ip* ip = (click_ip*)p->data();
    AdvertisementPacketheader* advh = (AdvertisementPacketheader*)(ip + 1);
    uint16_t flags = advh->flagsReserved;
    // create struct if we need to add
    Advertisement advStruct;
    advStruct.lifetime = advh->lifetime;
    advStruct.reg_lifetime = advh->lifetimeEx;
    advStruct.sequenceNum = advh->sequenceNum;
    advStruct.COA = advh->addressEx;
    advStruct.private_addr = ip->ip_src;
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

    if (((flags >> 6) & 1) == 1){
        click_chatter("busy bit set");
        // busy bit set so it has no point in registrating at this agent
        for (Vector<Advertisement>::iterator it = mobileNode->current_advertisements.begin(); it != mobileNode->current_advertisements.end(); ++it){
            // remove an entry from this host because we know he is busy
            if(it->private_addr == advStruct.private_addr && it->COA == advStruct.COA){
                mobileNode->current_advertisements.erase(it);
                break;
            }
        }
        return;
    }
    // modify current adv if needed. also extract the seq num
    // check if the adv is from the curr one if connected
    if(advh->address != mobileNode->home_private_addr){
        bool found = false;
        for (Vector<Advertisement>::iterator it = mobileNode->current_advertisements.begin(); it != mobileNode->current_advertisements.end(); ++it){
            // if we have an entry in the list. We can update it
            // this happens when we are connected but not necessairly with this agent
            if(it->private_addr == advStruct.private_addr && it->COA == advStruct.COA){
                found = true;
                // if the router is reset and i am connected to that one, re reg with new values
                if(advh->sequenceNum < 256 && advh->sequenceNum <= it->sequenceNum && mobileNode->curr_private_addr == it->private_addr){
                    if(mobileNode->curr_private_addr == it->private_addr){
                        click_chatter("reg source router reset");
                        source->makePacket(advStruct);
                    }
                    click_chatter("ADV UPDATE");
                    // update fields recording to the curr adv message,
                    it->lifetime = advh->lifetime;
                    it->reg_lifetime = advh->lifetimeEx;
                    it->sequenceNum = advh->sequenceNum;
                }
            }
        }
        if(found == false){
            mobileNode->current_advertisements.push_back(advStruct);
        }
    }
    bool advFromHome = false;
    if(advh->address == mobileNode->home_private_addr){
        advFromHome = true;
    }

    // change FA to HA
    if(mobileNode->connected == false && mobileNode->home && !advFromHome ){
        click_chatter("reg source swtich HA to FA");
        source->makePacket(advStruct);
        return;
    }
    // is there is a change FA to HA
    else if(mobileNode->connected == true && !mobileNode->home && advFromHome){
            click_chatter("reg source swtich FA to HA");
            source->makePacket(advStruct);
        }
}


// decrease lifetimes and act if needed
void AdvertisementsHandler::run_timer(Timer * timer) {
    click_chatter("Start timer advH");
    bool wasConnected = mobileNode->connected;
    bool hostConnectionLost = false;
    for (Vector<Advertisement>::iterator it = mobileNode->current_advertisements.begin(); it != mobileNode->current_advertisements.end();){
        if(it->lifetime == 0){
            // if my host is not active anymore
            if(wasConnected && it->private_addr == mobileNode->curr_private_addr){
                hostConnectionLost = true;
                mobileNode->connected = false;
            }
            mobileNode->current_advertisements.erase(it);
        }else{
            it->lifetime = it->lifetime-htons(1);
            it++;
        }
    }
    if(mobileNode->current_advertisements.empty()){
        mobileNode->advertisementReady = false;
    }
    if((!wasConnected || hostConnectionLost) && !mobileNode->current_advertisements.empty() && !mobileNode->home){
        click_chatter("Conection lost remake Request");
        source->makePacket(*mobileNode->current_advertisements.begin());
    }

    click_chatter("End timer advH");
    timer->reschedule_after_msec(1000);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AdvertisementsHandler)
