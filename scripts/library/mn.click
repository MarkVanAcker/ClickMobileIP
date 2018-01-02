// Mobile Node
// The input/output configuration is as follows:
//
// Packets for the network are put on output 0
// Packets for the host are put on output 1

// Input:
//	[0]: packets received from the network
//
// Output:
//	[0]: packets sent to the network
//	[1]: packets destined for the host itself


elementclass MobileNode {
	$address, $gateway, $home_agent |

	base :: MobileInfoList(MYADDR $address, PRADDR $gateway, PADDR $home_agent);
	regreq :: RegistrationRequestSource(MNBASE base);

	

	// Shared IP input path
	ip :: Strip(14)
		-> CheckIPHeader
		-> adv :: AdvFilter()
		-> rt :: LinearIPLookup( 
			$address:ip/32 0,
			$address:ipnet 1,
			0.0.0.0/0 2)
		-> ipc :: IPClassifier(src udp port 434,-)[1]
		-> [1]output;

	rt[1]	
		-> ipgw :: IPGWOptions($address)
		-> FixIPSrc($address)
		-> ttl :: DecIPTTL
		-> frag :: IPFragmenter(1500)
		-> arpq :: ARPQuerier($address)
		-> output;
	
	rt[2]
		-> SetGateway(MNBASE base)
		-> ipgw;

	ipgw[1]	-> ICMPError($address, parameterproblem)
		-> output;

	ttl[1]	-> ICMPError($address, timeexceeded)
		-> output;

	frag[1]	-> ICMPError($address, unreachable, needfrag)
		-> output;

	// incoming packets
	input
		-> HostEtherFilter($address)
		-> in_cl :: Classifier(12/0806 20/0001, 12/0806 20/0002, 12/0800)
		-> arp_res :: ARPResponder($address)
		-> output;

	in_cl[1]
		-> [1]arpq;

	in_cl[2]
		-> ip;


	AgentSolicitation(MNBASE base, MAXR 5)
		-> arpq;

	adv[1]
		-> CheckICMPHeader
		-> AdvertisementsHandler(MNBASE base, SOURCE regreq);

	ipc
		->regreq
		->arpq;

}
