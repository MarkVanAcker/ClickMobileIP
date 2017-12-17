#ifndef CLICK_MOBILEINFOLIST_HH
#define CLICK_MOBILEINFOLIST_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/etheraddress.hh>

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


class MobileInfoList: public Element {


public:

    MobileInfoList();
    ~MobileInfoList();

    // input packet that is encapsulated
    // output 0 encap for this agent
    // 0-0 for testing

    const char *class_name() const { return "MobileInfoList"; }
    const char *port_count() const { return "0/0"; }

    int configure(Vector<String>&, ErrorHandler*);


	bool home;
	bool connected;
    bool advertisementReady;
	uint16_t remainingConnectionTime;
	IPAddress curr_coa;
	IPAddress curr_private_addr;
	IPAddress home_public_addr;
	IPAddress home_private_addr;
    IPAddress myAddress;
	Vector<Advertisement> current_advertisements;

};





CLICK_ENDDECLS
#endif
