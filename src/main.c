#include "html_builder.h"
#include "server.h"
#include "string_stuff.h"
#include "request.h"
#include "box.h"
#include "router.h"
#define WITH_IMPL
#include "core.h"
#undef WITH_IMPL

#include "response.h"

const int PORT = 8001;

void index_route(Request *request, Response *response) {
    (void)request;

    FILE *file = fopen("static/test.html", "r");
    if (file == NULL) {
        debug("File static/test.html was not found");
        response->status_code = 404;
        response->body.type = STRING_RESPONSE;
        response->body.value.string = string_new("404 Not Found");
    } else {
        header_list_append(&response->header_list, header_create("Content-Type", "text/html; charset=utf-8"));
        response->body.type = FILE_RESPONSE;
        response->body.value.file = file;
        response->status_code = 200;
    }
}

void static_assets_route(Request *request, Response *response) {
    (void)request;

    debug("Serving a static file");

    StringView raw_path_sv = string_make_view(&request->path);
    // Remove the leading / (will always come)
    SV_SPLIT_STR(&raw_path_sv, "/static", empty_sv, rest);

    String full_path = string_new("static/assets");
    string_append_sv(&full_path, &rest);

    FILE *file = fopen(full_path.ptr, "r");
    if (file == NULL) {
        debugf("File `%s` was not found", full_path.ptr);
        response->status_code = 404;
        response->body.type = STRING_RESPONSE;
        response->body.value.string = string_new("404 Not Found");
    } else {
        header_list_append(&response->header_list, header_create("Content-Type", mime_type_for_file(&rest)));
        response->body.type = FILE_RESPONSE;
        response->body.value.file = file;
        response->status_code = 200;
    }
}

void favicon_route(Request *request, Response *response) {
    (void)request;
    FILE *file = fopen("static/assets/favicon.ico", "r");
    if (file == NULL) {
        debug("File `favicon.ico` was not found");
        response->status_code = 404;
        response->body.type = STRING_RESPONSE;
        response->body.value.string = string_new("404 Not Found");
    } else {
        StringView icon_sv = stringview_create("favicon.ico");
        header_list_append(&response->header_list, header_create("Content-Type", mime_type_for_file(&icon_sv)));
        response->body.type = FILE_RESPONSE;
        response->body.value.file = file;
    }
}

void not_found_route(Request *request, Response *response) {
    (void)request;

    response->status_code = 404;
    response->version = HTTP1_0;
    response->body.type = STRING_RESPONSE;
    response->body.value.string = string_new("<html><body><h1>Oops. That's not a valid url</h1></body><html>");

    header_list_append(&response->header_list, header_create("Content-Type", "text/html"));
}

int main(void) {
    Server server = {0};

    server_init(&server, 0x0, PORT);

     Router router = router_new();

     router_add_route(&router, "/static", &static_assets_route);
     router_add_exact_route(&router, "/favicon.ico", &favicon_route);
     router_add_exact_route(&router, "/", &index_route);
     router_add_route(&router, "/", &not_found_route);

     server_set_router(&server, router);

    server_start(&server);

    server_destroy(&server);
    return 0;
}
