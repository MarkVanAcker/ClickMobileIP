
	AgentAdvertiser(ADDAGENT 10.10.10.10 , COA 12.12.12.12, HA false, FA true, LTREG 3, LTADV 5, INTERVAL 1000)
	->EtherEncap(0x0800, 42:dd:23:31:ff:e1, 19:a3:ad:cf:ad:cc)
	->ToDump(req.dump)
	->Discard;

