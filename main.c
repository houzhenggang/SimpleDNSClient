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

char *
send_dns_query(const char *fqdn, in_addr_t ipv4)
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

	return dns_reply;
}

void
read_reply_dns(const char *dns_reply)
{
	struct dns_header *dns_header;
	dns_header = (struct dns_header *) dns_reply;
/*
	printf(" dns_reply->qdcount = %d\n", ntohs(dns_header->qdcount));
	printf(" dns_reply->ancount = %d\n", ntohs(dns_header->ancount));
	printf(" dns_reply->nscount = %d\n", ntohs(dns_header->nscount));
	printf(" dns_reply->arcount = %d\n", ntohs(dns_header->arcount));
*/
	if (!(ntohs(dns_header->ancount))) {
		puts(" Sem resposta!");
		return;
	}

}

int
main(int argc, char **argv)
{
	char *fqdn;
	char *ipv4;
	char *dns_reply;

	int c;

	fqdn = NULL;
	ipv4 = NULL;

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

	dns_reply = send_dns_query(fqdn, inet_addr(ipv4));
	read_reply_dns(dns_reply);

	free(dns_reply);
	return 0;
}

