	source::ForeignAgentReqProcess(FAGENT 20.20.20.20);

	RegistrationRequestSource(HADDR 10.10.10.10 , HAGENT 12.12.12.12, COA 15.15.15.15)
	->EtherEncap(0x0800, 42:dd:23:31:ff:e1, 19:a3:ad:cf:ad:cc)
	->Strip(14)
	->CheckIPHeader()
	->CheckUDPHeader()
	->source;



	source[0]
	->EtherEncap(0x0800, 19:a3:ad:cf:ad:cc,42:dd:23:31:ff:e1)
	->ToDump(req.dump)
	->Discard;


	source[1]
	->EtherEncap(0x0800, 19:a3:ad:cf:ad:cc,42:dd:23:31:ff:e1)
	->ToDump(req.dump)
	->Discard;

	//->RegistrationRequestReply(HAGENT 12.12.12.12)
	//->EtherEncap(0x0800, 19:a3:ad:cf:ad:cc,42:dd:23:31:ff:e1)
	//->ToDump(req.dump)
	//->Discard;
