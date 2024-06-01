#include "request.h"

HttpMethod http_method_from_str(const char *str) {
    usize str_len = strlen(str);
    #define X(method) else if (strncmp(str, #method, str_len) == 0) { return method; }
    if (false) { ASSERT(false, "Unreachable"); }
    HTTP_METHODS
    #undef X
    else {
        return UNKNOWN_METHOD;
    }
}

HttpMethod http_method_from_sv(const StringView *sv) {
    #define X(method) else if (strncmp(sv->ptr, #method, sv->length) == 0) { return method; }
    if (false) { ASSERT(false, "Unreachable"); }
    HTTP_METHODS
    #undef X
    else {
        return UNKNOWN_METHOD;
    }
}

String http_method_to_str(HttpMethod *method) {
    String str = {0};
    string_init(&str);
    switch (*method) {
        #define X(m) case m: string_append(&str, #m); break;
        HTTP_METHODS
        #undef X
    }

    return str;
}

HttpVersion http_version_from_str(const char *str) {
    usize str_len = strlen(str);
    #define X(version, name) else if (strncmp(str, name, str_len) == 0) { return version; }
    if (false) { ASSERT(false, "Unreachable"); }
    HTTP_VERSION
    #undef X
    else {
        return UNKNOWN_VERSION;
    }
}

HttpVersion http_version_from_sv(const StringView *sv) {
    #define X(version, name) else if (strncmp(sv->ptr, name, sv->length) == 0) { return version; }
    if (false) { ASSERT(false, "Unreachable"); }
    HTTP_VERSION
    #undef X
    else {
        return UNKNOWN_VERSION;
    }
}

String http_version_to_str(HttpVersion *version) {
    String str = {0};
    string_init(&str);
    switch (*version) {
        #define X(m, name) case m: string_append(&str, name); break;
        HTTP_VERSION
        #undef X
    }

    return str;
}

RequestBuilder request_builder_create(const char *str, int client_fd) {
    StringView sv = stringview_create(str);

    debugf("Request for client (fd: %d):", client_fd);

    RequestBuilder rb = {.request_body = sv, .client_fd = client_fd};
    rb.request.client_fd = client_fd;

    return rb;
}

void request_builder_destroy(RequestBuilder *rb) {
    request_destroy(&rb->request);
}

int request_builder_parse(RequestBuilder *rb) {
    // Parse the request-line
    SV_SPLIT_STR(&rb->request_body, "\r\n", request_line, rest);
    rb->request_body = rest;

    SV_SPLIT(&request_line, ' ', meth, rest_request_line);
    HttpMethod method = http_method_from_sv(&meth);

    if (method == UNKNOWN_METHOD) {
        return 1;
    }

    SV_SPLIT(&rest_request_line, ' ', request_path, ver);

    HttpVersion http_version = http_version_from_sv(&ver);

    SV_SPLIT(&request_path, '?', path, query_parameters);

    SV_SPLIT(&request_path, '#', actual_path, uri_fragment);

    String final_path = stringview_to_string(&actual_path);
    StringViewList path_components = string_split_all(&final_path, "/");
    stringview_list_unsafe_remove_first(&path_components);

    rb->request.method = method;
    rb->request.query = stringview_to_string(&query_parameters);
    rb->request.uri_fragment = stringview_to_string(&uri_fragment);
    rb->request.path = final_path;
    rb->request.path_components = path_components;
    rb->request.version = http_version;

    return 0;
}

void request_destroy(Request *req) {
    string_destroy(&req->path);
    string_destroy(&req->query);
    stringview_list_destroy(&req->path_components);
    string_destroy(&req->uri_fragment);
}
