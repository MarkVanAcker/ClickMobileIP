//
// Created by root on 25.11.17.
//

#include "homeAgentRegProcess.hh"
#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>

#include "foreignAgentReqProcess.hh"
#include "registrationRequestSource.hh"

CLICK_DECLS




CLICK_ENDDECLS

EXPORT_ELEMENT(ForeignAgentReqProcess)