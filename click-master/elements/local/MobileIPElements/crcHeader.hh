#ifndef CRCHEADER_HH
#define CRCHEADER_HH

// 4 bytes for the crc checksum for the eth header
struct EtherCrcHeader {
	uint32_t checksum;
};

#endif
