#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>

#include "registrationRequestSource.hh"

CLICK_DECLS
VisitorList::VisitorList(){}

VisitorList::~VisitorList()
{}

int VisitorList::VisitorList(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).read_mp("ADD", _homeAddress).complete() < 0) return -1;

	Timer *timer = new Timer(this);
	timer->initialize(this);
	timer->schedule_after_msec(1000);
	return 0;
}


bool
