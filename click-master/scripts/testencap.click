	bind::bindingsList()

	RegistrationRequestSource(HADDR 10.10.10.10 , HAGENT 12.12.12.12, COA 15.15.15.15)
	->EtherEncap(0x0800, 42:dd:23:31:ff:e1, 19:a3:ad:cf:ad:cc)
	->Strip(14)
	->CheckIPHeader()
	->CheckUDPHeader()
	->RegistrationRequestReply(HAGENT 12.12.12.12, BINDING bind)
	->EtherEncap(0x0800, 42:dd:23:31:ff:e1, 19:a3:ad:cf:ad:cc)
	->ToDump(req.dump)
	->Discard;



	//->EtherEncap(0x0800, 19:a3:ad:cf:ad:cc,42:dd:23:31:ff:e1)
	//->ToDump(req.dump)
	//->Discard;
