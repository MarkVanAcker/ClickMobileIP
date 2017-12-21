#ifndef CLICK_FOREIGNAGENTREPLYPROCESS_HH
#define CLICK_FOREIGNAGANTREPLYPROCESS_HH

#include <click/element.hh>
#include <click/ipaddress.hh>
#include "agentBase.hh"
CLICK_DECLS


class ForeignAgentReplyProcess: public Element {
    public:
        ForeignAgentReplyProcess();
        ~ForeignAgentReplyProcess();

        const char *class_name() const { return "ForeignAgentReplyProcess"; }
        const char *port_count() const { return "1/1"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);

        private:

            IPAddress _foreignAgent;
            AgentBase* _visitorList;
            short int _maxLifetime; // ??


};

CLICK_ENDDECLS
#endif
