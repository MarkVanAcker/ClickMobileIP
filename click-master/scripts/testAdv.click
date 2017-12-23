
	base :: AgentBase( PUBADDR 20.20.20.201, PRIVADDR 20.20.20.20);
	AgentAdvertiser(BASE base, HA false, FA true, LTREG 3, LTADV 5, INTERVAL 1000)
	->EtherEncap(0x0800, 42:dd:23:31:ff:e1, 19:a3:ad:cf:ad:cc)
	->ToDump(req.dump)
	->Discard;

