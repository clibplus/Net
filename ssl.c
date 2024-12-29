#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ssl.h"

void InitOpenSSL() {
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
}

void ssl_info_callback(int write_p, int version, int content_type, const void *buf, size_t len, SSL *ssl, void *arg) {
    printf("SSL Debug: write_p=%d, version=%d, content_type=%d, length=%zu\n", write_p, version, content_type, len);
}

/* Create a SSL */
SSL *CreateSSL(SSL_CTX *ctx) {
	return SSL_new(ctx);
}

/* Create a CTX */
SSL_CTX *CreateCTXContext() {
	return SSL_CTX_new(TLS_client_method());
}



void CleanOpenSSL(SSL *ssl, SSL_CTX *ctx) {
    SSL_free(ssl);
    SSL_CTX_free(ctx);
	EVP_cleanup();
}
