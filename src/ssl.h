#pragma once

#include <openssl/ssl.h>
#include <openssl/err.h>

void InitOpenSSL();
SSL *CreateSSL(SSL_CTX *ctx);
SSL_CTX *CreateCTXContext();
void CleanOpenSSL(SSL *ssl, SSL_CTX *ctx);
