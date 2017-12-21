#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>

#include "registrationRequestSource.hh"
#include "registrationRequestReply.hh"

CLICK_DECLS
RegistrationRequestSource::RegistrationRequestSource(){
    srand(time(NULL));
}

RegistrationRequestSource::~RegistrationRequestSource()
{}

int RegistrationRequestSource::configure(Vector<String> &conf, ErrorHandler *errh) {
    MobileInfoList* tempList;
    if (Args(conf, this, errh).read("MNBASE",ElementCastArg("MobileInfoList"),tempList).complete() < 0) return -1;

    _mobileNode = tempList;
	Timer* timer = new Timer(this);
	timer->initialize(this);
	timer->schedule_after_msec(1000);
    Timer* timer2 = new Timer(this);
	timer2->initialize(this);
    timers.push_back(timer);
    timers.push_back(timer2);

	return 0;
}

void RegistrationRequestSource::makePacket(Advertisement a){
    click_chatter("source make packet");
    // make the packet
    int packet_size = sizeof(struct RegistrationRequestPacketheader)+ sizeof(click_ip) + sizeof(click_udp);
    int headroom = sizeof(click_ether);
    WritablePacket *packet = Packet::make(headroom, 0, packet_size, 0);
    if(packet == 0) {
        click_chatter("Could not make packet");
        return;
    }
    memset(packet->data(), 0, packet->length());

    Request newReq;
    newReq.ipDst = a.private_addr;
    newReq.COA = a.COA;
    newReq.remainingLifetime = a.reg_lifetime;
    newReq.requestedLifetime = a.reg_lifetime;
	click_ip *iph = (click_ip *)packet->data();
    iph->ip_v = 4;
    iph->ip_hl = sizeof(click_ip) >> 2;
    iph->ip_len = htons(packet->length());
    iph->ip_id = htons(1);
    iph->ip_p = 17;
    iph->ip_ttl = 20;
    iph->ip_src = _mobileNode->myAddress;
    iph->ip_dst = a.private_addr;
    iph->ip_sum = click_in_cksum((unsigned char*)iph, sizeof(click_ip));

    packet->set_dst_ip_anno(iph->ip_dst); //not sure why it is used

    click_udp *udph = (click_udp*)(iph+1);
    uint16_t p = htons(1000 + (rand()%2000+1));
    newReq.port = p;
    udph->uh_sport = p; // anything
    udph->uh_dport = htons(434);
    udph->uh_ulen = htons(packet->length()-sizeof(click_ip));


    RegistrationRequestPacketheader* format = (RegistrationRequestPacketheader*)(udph+1);
    format->type = 1; //fixed
    format->flags = 0; //all flags 0   ||  4, 8, 16, 32 ?
    if(a.private_addr == _mobileNode->home_private_addr){
        click_chatter("source making a DEreg request");
        // update routing table
        _mobileNode->curr_private_addr = _mobileNode->home_private_addr;
        _mobileNode->curr_coa = a.COA;
        _mobileNode->remainingConnectionTime = 0; // should not matter
        format->lifetime = 0;
    }else{
        click_chatter("source making a reg request");
        format->lifetime = a.reg_lifetime;
        format->lifetime = format->lifetime-htons(1);
    }
    format->homeAddr = _mobileNode->myAddress;
    format->homeAgent = _mobileNode->home_public_addr;
    format->coAddr = a.COA;
    unsigned int id1  = rand() % (2147483647);
    unsigned int id2  = rand() % (2147483647);
    format->id1 = htonl(id1); // max 32 bit number (unsigned) if we want to secure the messages
    format->id2 = htonl(id2);
    newReq.id1 = id1;
    newReq.id2 = id2;


    udph->uh_sum = click_in_cksum_pseudohdr(click_in_cksum((unsigned char*)udph, packet_size-sizeof(click_ip)),
    iph, packet_size - sizeof(click_ip));

    currentRequests.push_back(newReq);

    output(0).push(packet);
}

void RegistrationRequestSource::push(int, Packet *p) {
    click_ip *iph = (click_ip *)p->data();
    click_udp *udph= (click_udp*)(iph+1);
    RegistrationRequestReplyPacketheader *format = (RegistrationRequestReplyPacketheader*)(udph+1);
    //
    // check pendings and accept occerdingly
    //
    if(format->type == 3){
        if(format->code == 0 || format->code == 1){ // code 1 should not be used
            for (Vector<Request>::iterator it = currentRequests.begin(); it != currentRequests.end(); it++){
                if(format->id1 == it->id1 && format->id2 == it->id2 && udph->uh_dport == it->port){
                    // found corresponding request
                    if(format->lifetime == 0){
                        _mobileNode->home = true;
                        _mobileNode->remainingConnectionTime = 0; // doenst really matter
                        _mobileNode->curr_private_addr = it->ipDst;
                        _mobileNode->curr_coa = it->COA;
                    }else{
                        _mobileNode->home = false;
                        _mobileNode->connected = true;
                        _mobileNode->curr_private_addr = it->ipDst;
                        _mobileNode->curr_coa = it->COA;
                        uint16_t lifetimeReq = it->requestedLifetime;
                        uint16_t lifetimeResponse = format->lifetime;
                        uint16_t diff = lifetimeReq - lifetimeResponse;
                        uint16_t lifetime = it->remainingLifetime - diff;
                        _mobileNode->remainingConnectionTime = lifetime;
                        timers.back()->reschedule_after_msec(1000);
                    }
                }
            }
        }
    }
    return;
}

void RegistrationRequestSource::run_timer(Timer *timer){
    if(timer == (*timers.begin())){
        for(Vector<Request>::iterator it = currentRequests.end()-1; it != currentRequests.begin()-1; it--) {
            it->remainingLifetime = it->remainingLifetime-htons(1);
            if(it->remainingLifetime == 0){
                currentRequests.erase(it);
            }
        }
        timer->reschedule_after_msec(1000);
    }else{
        if(_mobileNode->remainingConnectionTime == 0){
            timer->reschedule_after_msec(1000);
            return;
        }
        _mobileNode->remainingConnectionTime = _mobileNode->remainingConnectionTime-htons(1);
        if(_mobileNode->remainingConnectionTime == 0 && !_mobileNode->home){
            _mobileNode->connected = false;
        }
        if(_mobileNode->remainingConnectionTime == 2 && _mobileNode->connected && !_mobileNode->home){
            // we want to re register if the host is still active (find adv)
            for(Vector<Advertisement>::iterator it = _mobileNode->current_advertisements.begin(); it != _mobileNode->current_advertisements.end(); it++) {
                if(it->private_addr  == _mobileNode->curr_private_addr && it->COA == _mobileNode->curr_coa){
                    makePacket(*it);
                    break;
                }
            }
        }

        timer->reschedule_after_msec(1000);
    }

}

CLICK_ENDDECLS
EXPORT_ELEMENT(RegistrationRequestSource)
