#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#	define DPRINT(x) fprintf(stderr, " [DEBUG] %s\n", x);
#else
#	define DPRINT(x)
#endif

void erro_fatal(const char * msg)
{
	DPRINT(msg)
	exit(EXIT_FAILURE);
}

void usage(const char *executable)
{
	printf(" Simple DNS Client v0.1 by Kingm0b_\n\n"
		" Usage: %s [-h] -d <fqdn> -s <ipv4 address>\n"
		"       -h: Print this help.\n"
		"       -d: The FQDN for resolution.\n"
		"       -s: IPv4 address of DNS server.\n\n"
		" Example: %s -d www.example.com -s 8.8.8.8\n\n", executable, executable);
}