#ifndef CLICK_ADVERTISEMENTSHANDLER_HH
#define CLICK_ADVERTISEMENTSHANDLER_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include "mobileinfolist.hh"
#include "registrationRequestSource.hh"
#include "agentAdvertisements.hh"


CLICK_DECLS



class AdvertisementsHandler: public Element {
    public:
        AdvertisementsHandler();
        ~AdvertisementsHandler();

        const char *class_name() const { return "AdvertisementsHandler"; }
        const char *port_count() const { return "1/0"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);
        void run_timer(Timer*);

    private:
        MobileInfoList* _mobileNode;
		RegistrationRequestSource* _source;

};



CLICK_ENDDECLS
#endif
