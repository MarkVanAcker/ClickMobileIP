	visit::VisitorList(ADD 20.20.20.20);	
	source::ForeignAgentReqProcess(FAGENT 20.20.20.20 , VISITOR visit);

	RegistrationRequestSource(HADDR 10.10.10.10 , HAGENT 12.12.12.12, COA 15.15.15.15)
	->EtherEncap(0x0800, 42:dd:23:31:ff:e1, 19:a3:ad:cf:ad:cc)
	->Strip(14)
	->source;


	// can use 1 todumpt to check the errcode 66 return

	source[0]
	->EtherEncap(0x0800, 19:a3:ad:cf:ad:cc,42:dd:23:31:ff:e1)
	->ToDump(req.dump)
	->Discard;


	source[1]
	->EtherEncap(0x0800, 19:a3:ad:cf:ad:cc,42:dd:23:31:ff:e1)
	//->ToDump(req.dump)
	->Discard;

