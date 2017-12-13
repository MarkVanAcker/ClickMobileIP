
	AgentSolicitation(ADDRNODE 10.10.10.10, MAX 3)
	->EtherEncap(0x0800, 42:dd:23:31:ff:e1, 19:a3:ad:cf:ad:cc)
	->ToDump(req.dump)
	->Discard;

