#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "dns_protocol.h"
#include "dns_encode.h"
#include "utils.h"

struct dns_response send_dns_query(const char *fqdn, in_addr_t ipv4)
{
	char buffer[1024];
	int fd_sock;

	char *dns_consult;
	char *dns_reply;

	size_t fqdn_encoded_size;
	size_t query_len;
	size_t dns_header_size;

	ssize_t bytes_tx;
	ssize_t bytes_rx;

	struct dns_header *dns_header;
	struct dns_question *dns_question;

	struct dns_response dns_response;

	struct sockaddr_in dns_server;
	struct sockaddr_in remote_socket;

	fqdn_encoded_size = strlen(fqdn) + 2;
	dns_header_size = sizeof(struct dns_header);

	query_len = sizeof(struct dns_header) + sizeof(struct dns_question)
		    + fqdn_encoded_size;

	dns_consult = calloc(query_len, sizeof(char));
	dns_reply   = calloc(DNS_REPLY_MAX_SIZE, sizeof(char));

	if (!dns_consult || !dns_reply)
		erro_fatal("main: calloc");

	dns_header = (struct dns_header *) dns_consult;
	dns_question = (struct dns_question *) &dns_consult[dns_header_size + fqdn_encoded_size];

	dns_header->id		= htons('V');
	dns_header->rd		= 1;
	dns_header->qdcount	= htons(1);

	dns_encode_fqdn(fqdn, fqdn_encoded_size - 2, &dns_consult[dns_header_size]);

	dns_question->qtype	= htons(1);
	dns_question->qclass	= htons(1);

	bzero(&dns_server, sizeof(dns_server));
	dns_server.sin_family		= AF_INET;
	dns_server.sin_port		= htons(53);
	dns_server.sin_addr.s_addr	= ipv4;

	fd_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd_sock == -1)
		erro_fatal("send_dns_query: socket()");

	bytes_tx = sendto(fd_sock, dns_consult, query_len, 0,
			 (struct sockaddr *)&dns_server, sizeof(dns_server));

	if (bytes_tx == -1)
		erro_fatal("send_dns_query: sendto()");

	socklen_t i;
	bytes_rx = recvfrom(fd_sock, dns_reply, DNS_REPLY_MAX_SIZE, 0,
			   (struct sockaddr*)&remote_socket, (socklen_t*)&i);

	if (bytes_rx == -1)
		erro_fatal("send_dns_query: recvfrom()");

	close(fd_sock);
	free(dns_consult);

	dns_response.dns_reply = dns_reply;
	dns_response.dns_reply_bytes = bytes_rx;

	return dns_response;
}

void read_reply_dns(const u_char *dns_reply, size_t dns_reply_size)
{
	const struct dns_header *dns_header = NULL;
	struct dns_question *dns_question;
	const u_char *p = NULL;
	const u_char *ipv4 = NULL;

	dns_header = (struct dns_header *) dns_reply;

	if ((ntohs(dns_header->qdcount)) != 1) {
		return;
	}

	if (!(ntohs(dns_header->ancount))) {
		puts(" Host not found!");
		return;
	}

	p = dns_reply + sizeof(struct dns_header);

	for (; *p != '\0'; p++);

	p += 1;
	if (p[1] == DNS_A_RECORD) {
		ipv4 = p + 16;
		printf(" IP: %d.%d.%d.%d\n", *ipv4, *(ipv4+1), *(ipv4+2), *(ipv4+3));
	}
}

int main(int argc, char **argv)
{
	char *fqdn;
	char *ipv4;

	int c;

	fqdn = NULL;
	ipv4 = NULL;

	struct dns_response dns_response;

	opterr	= 0;
	while ((c = getopt(argc, argv, "d:s:h")) != -1) {
		switch(c) {
			case 'd':
				fqdn = optarg;
				break;
			case 's':
				ipv4 = optarg;
				break;
			case 'h':
				usage(argv[0]);
				exit(EXIT_FAILURE);
				break;
			case '?':
			default:
				usage(argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	if (!fqdn || !ipv4) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	dns_response = send_dns_query(fqdn, inet_addr(ipv4));
	read_reply_dns(dns_response.dns_reply, dns_response.dns_reply_bytes);

	free(dns_response.dns_reply);
	return 0;
}

