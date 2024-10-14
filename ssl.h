#pragma once

#include <openssl/ssl.h>
#include <openssl/err.h>

void InitOpenSSL();
SSL_CTX *CreateSSLContext();
void CleanOpenSSL();
