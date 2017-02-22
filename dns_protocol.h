#include <sys/types.h>

enum dns_query_type {
	DNS_A_RECORD = 0x01,
	DNS_CNAME_RECORD = 0x05,
	DNS_MX_RECORD = 0x0f
};

/*
 * Members of bit-fields inverted for big-endian adequation 
 * Only *little endian* architectures
*/
struct dns_header {
	/* Two bytes */
	u_int16_t id;

	/* One byte */
	u_int16_t rd:1;
	u_int16_t tc:1;
	u_int16_t aa:1;
	u_int16_t opcode:4;
	u_int16_t qr:1;

	/* One byte */
	u_int16_t rcode:4;
	u_int16_t z:3;
	u_int16_t ra:1;

	/* Eight bytes */
	u_int16_t qdcount;
	u_int16_t ancount;
	u_int16_t nscount;
	u_int16_t arcount;
};

/*
                                    1  1  1  1  1  1
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      ID                       |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    QDCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ANCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    NSCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ARCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

*/

struct dns_question {
	u_int16_t qtype;
	u_int16_t qclass;
};

struct dns_response_packet {
	char *name;
	u_int16_t type;
	u_int16_t class;
	u_int32_t ttl;
	u_int16_t rdlength;
	char *rdata;
};

struct dns_packet {
	struct dns_header header;
	char *data;
	u_int16_t data_size;
};
