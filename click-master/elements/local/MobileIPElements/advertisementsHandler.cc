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

int AdvertisementsHandler::configure(Vector<String> &conf,ErrorHandler *errh) {
    MobileInfoList* tempList;
    if (Args(conf, this, errh).read("MNLIST",ElementCastArg("MobileInfoList"),tempList).complete() < 0) return -1;

    _mobileNode = tempList;
    Timer *timer = new Timer(this);
    timer->initialize(this);
    timer->schedule_after_msec(1000);
    return 0;
}


// expects node Sollicitation packet
// sends an extra packet, we do not reset the timer
void AdvertisementsHandler::push(int, Packet *p) {

        output(0).push(p);

}


// make packet and increase seq num
void AdvertisementsHandler::run_timer(Timer * timer) {

    // random term to make sure 2 hosts messages will not interfere
    timer->reschedule_after_msec(1000+((rand()%70)-35));
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AdvertisementsHandler)
