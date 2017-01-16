#include <sys/types.h>

void dns_encode_fqdn(const char *fqdn, size_t size, char *fqdn_encoded)
{
	for (int i = 1; i <= size; i++) {
		fqdn_encoded[i] = fqdn[i - 1];
	}

	fqdn_encoded[0] = '.';
	fqdn_encoded[size + 1] = '.';

	int c = 0;
	for (int i = size + 1; i >= 0; i--) {
		if (fqdn_encoded[i] == '.') {
			fqdn_encoded[i] = c;
			c = 0;
		} else {
			c++;
		}
	}
}
