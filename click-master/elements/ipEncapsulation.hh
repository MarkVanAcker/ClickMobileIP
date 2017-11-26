#ifndef CLICK_IPENCAPSULATION_HH
#define CLICK_IPENCAPSULATION_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>

CLICK_DECLS


class IpEncapsulation: public Element {
    public:
        IpEncapsulation();
        ~IpEncapsulation();

        const char *class_name() const { return "IpEncapsulation"; }
        const char *port_count() const { return "1/1"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);
        IPAddress getDesitination(IPAddress);

    private:
        IPAddress _tunnelAddres;
        //Bindings* _bindingsList;




};


CLICK_ENDDECLS
#endif
