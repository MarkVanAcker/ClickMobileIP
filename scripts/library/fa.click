// Home Agent
// The input/output configuration is as follows:
//
// Input:
//	[0]: packets received on the private network
//	[1]: packets received on the public network
//
// Output:
//	[0]: packets sent to the private network
//	[1]: packets sent to the public network
//	[2]: packets destined for the router itself

elementclass Agent {
	$private_address, $public_address, $gateway |

	bind::AgentBase(PUBADDR $public_address,PRIVADDR $private_address, LFREG 25);

	// Shared IP input path and routing table
	ip :: Strip(14)
		-> CheckIPHeader
		-> soli :: SolicitationFilter
		-> rt :: StaticIPLookup(
					$private_address:ip/32 0,
					$public_address:ip/32 0,
					$private_address:ipnet 1,
					$public_address:ipnet 2,
					0.0.0.0/0 $gateway 2);
	


	// ARP responses are copied to each ARPQuerier and the host.
	arpt :: Tee (2);
	
	// Input and output paths for interface 0
	input
		-> HostEtherFilter($private_address)
		-> private_class :: Classifier(12/0806 20/0001, 12/0806 20/0002, 12/0800)
		-> ARPResponder($private_address)
		-> output;

	private_arpq :: ARPQuerier($private_address)
		-> output;

	private_class[1]
		-> arpt
		-> [1]private_arpq;

	private_class[2]
		-> Paint(1)
		-> ip;

	// Input and output paths for interface 1
	input[1]
		-> HostEtherFilter($public_address)
		-> public_class :: Classifier(12/0806 20/0001, 12/0806 20/0002, 12/0800)
		-> ARPResponder($public_address)
		-> [1]output;

	public_arpq :: ARPQuerier($public_address)
		-> [1]output;

	public_class[1]
		-> arpt[1]
		-> [1]public_arpq;

	public_class[2]
		-> Paint(2)
		-> ip;

	// Local delivery
	rt[0]
		-> ipdecap :: IpDecap(BASE bind)
		-> ipc :: IPClassifier(src udp port 434 or dst udp port 434,-)[1]
		-> [2]output
	
	// Forwarding paths per interface
	rt[1]
		-> DropBroadcasts
		-> ipenc :: IpEncapsulation(BINDING bind)
		-> private_paint :: PaintTee(1)
		-> private_ipgw :: IPGWOptions($private_address)
		-> FixIPSrc($private_address)
		-> private_ttl :: DecIPTTL
		-> private_frag :: IPFragmenter(1500)
		-> private_arpq;
	
	private_paint[1]
		-> ICMPError($private_address, redirect, host)
		-> rt;

	private_ipgw[1]
		-> ICMPError($private_address, parameterproblem)
		-> rt;

	private_ttl[1]
		-> ICMPError($private_address, timeexceeded)
		-> rt;

	private_frag[1]
		-> ICMPError($private_address, unreachable, needfrag)
		-> rt;
	
	

	rt[2]
		-> DropBroadcasts
		-> public_paint :: PaintTee(2)
		-> public_ipgw :: IPGWOptions($public_address)
		-> FixIPSrc($public_address)
		-> public_ttl :: DecIPTTL
		-> public_frag :: IPFragmenter(1500)
		-> public_arpq;
	
	public_paint[1]
		-> ICMPError($public_address, redirect, host)
		-> rt;

	public_ipgw[1]
		-> ICMPError($public_address, parameterproblem)
		-> rt;

	public_ttl[1]
		-> ICMPError($public_address, timeexceeded)
		-> rt;

	public_frag[1]
		-> ICMPError($public_address, unreachable, needfrag)
		-> rt;


//IP in IP berichten worden direct op het publieke netwerk geduwd na aanmaak.
	ipenc [1]
		-> public_arpq;

//soorten Requests die hier binnenkomen:
//1. Registration requests van privé netwerk bestemd voor agent zelf.
//1.1 Ze zijn van een thuisnode en hiervoor genereer je direct een reply terug.
//1.2 Ze zijn van een visitornode en deze stuur je door naar zijn HA.
//2. Registration requests van public netwerk voor jezelf. Deze moeten altijd beantwoord en teruggestuurd worden.
//3. Registration reply van een public netwerk voor jezelf. Deze worden doorverwezen naar de visitor node.

	ipc
		-> mipfilter :: MobileIPFilter(AGBASE bind)
		-> Discard

	mipfilter[1]
		-> ForeignAgentReplyProcess(AGBASE bind)
		-> private_arpq;

	mipfilter[2]
		-> regrep :: RegistrationRequestReply(BINDING bind) //moet via RT terugsturen in plaats van op te splitsen in 2 outputs, moet ook berichten kunnen doorsturen als HAaddr != $public ADDR
		-> public_arpq;


	mipfilter[3]
		-> farp :: ForeignAgentReqProcess(BASE bind)
		-> private_arpq;


		farp[1]
			->public_arpq;


//infobase moet weet hebben van eigen adres zodat hij kan beslissen waartoe het packet behoort
	
	regrep[1]
		-> private_arpq;


//Solicitation requests komen hier binnen en zijn 100% geldig. Er wordt een Advertisement gemaakt en doorgestuurd op het prive netwerk.
//publieke packages worden gedropt

	soli[1]
		-> CheckPaint(1)
		-> CheckICMPHeader
		-> AgentAdvertiser(BASE bind, HA true, FA true, LTADV 60, INTERVAL 15000)
		->EtherEncap(0x0800, $private_address:eth, FF:FF:FF:FF:FF:FF)
		-> output;


	ipdecap[1]
		-> CheckIPHeader
		-> private_arpq;

}
