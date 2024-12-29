#pragma once

#include <str.h>
#include <arr.h>
#include <map.h>
#include "ssl.h"

#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

typedef enum Req_T {
	__GET__			= 0x023001,
	__POST__		= 0x023002
} Req_T;

typedef enum StatusCode {
    CONTINUEE                       = 100,
    SWITCH_PROTOCOL                 = 101,
    PROCESSING                      = 102,
    EARLY_HINT                      = 103,

    OK                              = 200,
    CREATED                         = 201,
    ACCEPTED                        = 202,
    NON_AUTHORIZED_INFO             = 203,
    NO_CONTENT                      = 204,
    RESET_CONTENT                   = 205,
    PARTIAL_CONTENT                 = 206,
    MULTI_STATUS                    = 207,
    ALREADY_REPRORTED               = 208,
    IM_USED                         = 226,

    MULTIPLE_CHOICES                = 300,
    MOVED_PERMANENTLY               = 301,
    FOUND                           = 302,
    SEE_OTHER                       = 303,
    NOT_MODIFIED                    = 304,
    USE_PROXY                       = 305,
    SWITCH_PROXY                    = 306,
    TEMP_REDIRECT                   = 307,
    PERM_REDIRECT                   = 308,

    BAD_REQUEST                     = 400,
    UNAUTHORIZED                    = 401,
    PAYMENT_REQUIRED                = 402,
    FORBIDDEN                       = 403,
    NOT_FOUND                       = 404,
    METHOD_NOT_ALLOWED              = 405,
    NOT_ACCEPTABLE                  = 406,
    PROXY_AUTH_REQUIRED             = 407,
    REQUIRE_TIMEOUT                 = 408,
    CONFLICT                        = 409,
    GONE                            = 410,
    LENGTH_REQUIRED                 = 411,
    PRECONDITION_FAILED             = 412,
    PAYLOAD_TOO_LARGE               = 413,
    URI_TOO_LONG                    = 414,
    UNSUPPORTED_MEDIA_TYPE          = 415,
    RANGE_NOT_SATISFIABLE           = 416,
    EXPECTATION_FAILED              = 417,
    IM_A_TEAPOT                     = 418,
    MISDIRECTED_REQUEST             = 421,
    UNPROCESSABLE_ENTITY            = 422,
    LOCKED                          = 423,
    FAILED_DEPENDENCY               = 424,
    TOO_EARLY                       = 425,
    UPGRADE_REQUIRED                = 426,
    PROCONDITION_REQUIRED           = 428,
    TOO_MANY_REQUEST                = 429,
    REQ_HEADER_FIELD_TOO_LARGE      = 431,
    UNAVAILABLE_FOR_LEGAL_REASON    = 451,
    
    INTERNAL_SERVER_ERR             = 500,
    NOT_IMPLEMENTED                 = 501,
    BAD_GATEWAY                     = 502,
    SERVER_UNAVAILABLE              = 503,
    GATEWAY_TIMEOUT                 = 504,
    HTTP_VERSION_NOT_SUPPORTED      = 505,
    VARIANT_ALSO_NEGOTIATES         = 506,
    INSUFFICIENT_STORAGE            = 507,
    LOOP_DETECTED                   = 508,
    NOT_EXTENDED                    = 510,
    NETWORK_AUTHENTICATION_REQUIRED = 511
} StatusCode;

typedef struct HTTPClient {
	String		Hostname;
	String 		Port;
	String		URL_Route;
    String      Route;
	Map			Headers;

	SSL 		*SSL;
	SSL_CTX		*CTX;
	int			ServerFD;
} HTTPClient;

typedef struct HTTPClientResponse {
	String 		Route;
	String		FullRoute;
	StatusCode	StatusCode;
	Map			Headers;
	Map			Query;
	String		Body;
} HTTPClientResponse;

//
//
//
//
Array ParseURL(const char *u);

//
//
//
//
HTTPClientResponse RequestURL(const String URL, const Map h, const Req_T reqt);

//
//
//
//
int SendHTTPGetReq(HTTPClient *c);

//
//
//
//
int CreateHTTPSocket(HTTPClient *c);

HTTPClientResponse RetrieveHTTPResponse(HTTPClient *c);

int ExtractRawTraffic(HTTPClient *c, HTTPClientResponse *r);


