#ifndef CLICK_ADVERTISEMENTSHANDLER_HH
#define CLICK_ADVERTISEMENTSHANDLER_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include "mobileinfolist.hh"
#include "advertisementsHandler.hh"

CLICK_DECLS

struct Advertisement{
		uint16_t lifetime;
		uint16_t reg_lifetime;
		uint16_t sequenceNum;
		IPAddress COA;
		IPAddress private_addr;
		bool ha;
		bool fa;
};

class AdvertisementsHandler: public Element {
    public:
        AdvertisementsHandler();
        ~AdvertisementsHandler();

        const char *class_name() const { return "AdvertisementsHandler"; }
        const char *port_count() const { return "1/0-1"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);
        void run_timer(Timer*);

    private:
        MobileInfoList* _mobileNode;
		RegistrationRequestSource* _source;
        Vector<Advertisement> current_advertisements;
		Vector<Timer> current_timers;


};



CLICK_ENDDECLS
#endif
