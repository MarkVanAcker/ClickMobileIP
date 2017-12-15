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
    // create struct if we need to add
    Advertisement advStruct;
    Advertisement.lifetime = advh->lifetime;
    Advertisement.reg_lifetime = advh->lifetimeEx;
    Advertisement.sequenceNum = advh->sequenceNum;
    Advertisement.COA = advh->addressEx;
    Advertisement.private_addr = advh->adress;
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
                // make packet (req) for this advert _source
                // stuff
            }else{
                // update fields recording to the curr adv message,

            }



        }
     }


}


// make packet and increase seq num
void AdvertisementsHandler::run_timer(Timer * timer) {

    // random term to make sure 2 hosts messages will not interfere
    timer->reschedule_after_msec(1000+((rand()%70)-35));
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AdvertisementsHandler)
