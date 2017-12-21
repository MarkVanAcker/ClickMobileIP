	


	base :: MobileInfoList(MYADDR 15.15.15.15, PRADDR 10.10.10.10, PADDR 10.10.10.101);
	basee :: AgentBase( PUBADDR 20.20.20.201, PRIVADDR 20.20.20.20);
	regreq :: RegistrationRequestSource(MNBASE base);

	AgentSolicitation(MNBASE base, MAXR 5)
			-> Discard;


	AgentAdvertiser(HA true, FA true, LTREG 10, LTADV 3, INTERVAL 2000, ADDAGENT 20.20.20.20, COA 20.20.20.201)
	->a::AdvFilter()[1]
	-> AdvertisementsHandler(MNBASE base, SOURCE regreq);

	regreq[0]
		->aa::AdvFilter()[0]
		->Print("called")
		->f::ForeignAgentReqProcess(BASE basee)[1]
		->EtherEncap(0x0800, 42:dd:23:31:ff:e1, 19:a3:ad:cf:ad:cc)
		->ToDump(req.dump)
		->Discard;

	a[0]
	->Print("geen mobile ip")
	->Discard;

	aa[1]
	->Print("gek")
	->Discard;
	f[0]
	->Print("mmh")
	->Discard

