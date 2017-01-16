#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "dns_protocol.h"
#include "dns_encode.h"

void erro_fatal(const char * msg) {
#ifdef DEBUG
    fprintf(stderr, " ERRO: %s\n", msg);
#endif
    exit(EXIT_FAILURE);
}

int main(void)
{
	char fqdn[] = "www.icann.org";
	char buffer[1024];
	int fd_sock;

	size_t fqdn_encoded_size;
	size_t query_len;

	char *dns_consult;

	struct dns_header *dns_header;
	struct dns_question *dns_question;
	struct sockaddr_in dns_server;
	struct sockaddr_in remote_socket;

	fqdn_encoded_size = strlen(fqdn) + 2;
	query_len = sizeof(struct dns_header) + sizeof(struct dns_question) + fqdn_encoded_size;

	dns_consult = calloc(query_len, sizeof(char));
	if (!dns_consult)
		erro_fatal("main: calloc");

	dns_header = (struct dns_header *) dns_consult;
	dns_question = (struct dns_question *) &dns_consult[sizeof(struct dns_header) + fqdn_encoded_size];

	dns_header->id = 666;
	dns_header->rd = 1;
	dns_header->qdcount = 1;

	dns_encode_fqdn(fqdn, fqdn_encoded_size - 2, &dns_consult[sizeof(struct dns_header)]);

	dns_question->qtype = 1;
	dns_question->qclass = 1;

	bzero(&dns_server, sizeof(dns_server));
	dns_server.sin_family = AF_INET;
	dns_server.sin_port = htons(53);
	dns_server.sin_addr.s_addr = inet_addr("8.8.8.8");

	fd_sock = socket(AF_INET, SOCK_DGRAM, 0);
	sendto(fd_sock, dns_consult, query_len,	0, (struct sockaddr *)&dns_server, sizeof(dns_server));

	socklen_t i;
	recvfrom (fd_sock, buffer, 1024, 0, (struct sockaddr*)&remote_socket, (socklen_t*)&i);

	return 0;
}

