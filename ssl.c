#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ssl.h"

void InitOpenSSL() {
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();
}

SSL_CTX *CreateSSLContext() {
	const SSL_METHOD *Method;
	Method = SSLv23_client_method();

	return SSL_CTX_new(Method);
}

void CleanOpenSSL() {
	EVP_cleanup();
}
