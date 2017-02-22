#ifndef _DNS_CLIENT_UTILS_H

#define DNS_REPLY_MAX_SIZE 512

extern void erro_fatal(const char *);
extern void usage(const char *);

struct dns_response {
	u_char *dns_reply;
	size_t dns_reply_bytes;
};

#endif