#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "mobileinfolist.hh"


CLICK_DECLS
MobileInfoList::MobileInfoList(){}

MobileInfoList::~MobileInfoList(){}

int MobileInfoList::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).read_m("MYADDR", myAddress).read_m("PRADDR", home_private_addr).read_m("PADDR",home_public_addr).complete() < 0) return -1;
		connected = true;
        remainingConnectionTime = 0;
        advertisementReady = false;
		curr_private_addr = home_private_addr;
		home = true;
        return 0;

    }





CLICK_ENDDECLS
EXPORT_ELEMENT(MobileInfoList)
