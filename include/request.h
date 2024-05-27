#ifndef REQUEST_H
#define REQUEST_H
#include "string_stuff.h"

#define HTTP_METHODS\
    X(UNKNOWN_METHOD) \
    X(GET) \
    X(PUT) \
    X(POST) \
    X(DELETE)

typedef enum {
    #define X(method) method,
    HTTP_METHODS
    #undef X
} HttpMethod;

HttpMethod http_method_from_str(const char *);
HttpMethod http_method_from_sv(const StringView*);
String http_method_to_str(HttpMethod*);

#define HTTP_VERSION\
    X(UNKNOWN_VERSION, "unknown_http_version") \
    X(HTTP1_0, "HTTP/1.1")

typedef enum {
    #define X(version, name) version,
    HTTP_VERSION
    #undef X
} HttpVersion;

HttpVersion http_version_from_str(const char *);
HttpVersion http_version_from_sv(const StringView *);
String http_version_to_str(HttpVersion*);

typedef struct {
    int client_fd;
    HttpMethod method;
    String path;
    String uri_fragment;
    String query;
    HttpVersion version;
} Request;

typedef struct {
    StringView request_body;
    int client_fd;
    Request request;
} RequestBuilder;

RequestBuilder request_builder_create(const char *str, int client_fd);
void request_builder_destroy(RequestBuilder *);
int request_builder_parse(RequestBuilder *);

void request_destroy(Request *);

#endif
