#ifndef CLICK_MOBILEINFOLIST_HH
#define CLICK_MOBILEINFOLIST_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/etheraddress.hh>

CLICK_DECLS





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
	uint16_t remainingConnectionTime;
	IPAddress curr_coa;
	IPAddress curr_private_addr;
	IPAddress home_public_addr;
	IPAddress home_private_addr;
	//Vector<Advertisement*> current_advertisements;

};





CLICK_ENDDECLS
#endif
