#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "setgateway.hh"

CLICK_DECLS
SetGateway::SetGateway(){
    srand(time(NULL));
}

SetGateway::~SetGateway(){}

int SetGateway::configure(Vector<String> &conf,ErrorHandler *errh) {
  MobileInfoList* tempList;
  if (Args(conf, this, errh)
      .read_m("MNBASE",ElementCastArg("MobileInfoList"),tempList)
      .complete() < 0) return -1;


  mobileNode = tempList;

	return 0;
}





// no functionality so far
void SetGateway::push(int, Packet *p) {
	p->set_dst_ip_anno(mobileNode->curr_private_addr);
	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(SetGateway)
