#include "html_builder.h"
#include "server.h"
#include "string_stuff.h"
#include "request.h"
#include "box.h"
#include "router.h"
#include "slow_dict.h"
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

bool user_id_route_matcher(const StringViewList *path_components, SlowDict *dyn) {
    if (path_components->length != 2) { return false; }

    if (stringview_compare_str(&path_components->data[0], "users")) {
        StringView *sv = &path_components->data[1];

        bool is_digit = true;
        for (usize i = 0; i < sv->length; i++) {
            char c = sv->ptr[i];
            if (c >= '0' && c <= '9') { continue; }
            is_digit = false;
            break;
        }

        if (is_digit) {
            String value = stringview_to_string(sv);
            slow_dict_insert(dyn, string_new("id"), value);
        }

        return is_digit;
    }
    return false;
}

void user_id_route(Request *request, Response *response) {
    (void) request;
    (void) response;

    response->status_code = 200;
    response->version = HTTP1_0;
    response->body.type = STRING_RESPONSE;
    HtmlBuilder hb = html_builder_new();
    html_builder_make_node(&hb, "html");
    html_builder_open_node(&hb);

    html_builder_make_node(&hb, "body");
    html_builder_open_node(&hb);

    html_builder_make_node(&hb, "h1");
    html_builder_open_node(&hb);

    html_builder_add_text(&hb, "Page for user: ");

    const String *id = slow_dict_get(&request->dynamic_data, "id");
    if (id) {
        html_builder_add_text(&hb, id->ptr);
    } else {
        html_builder_add_text(&hb, "No User");
    }

    html_builder_close_node(&hb, "h1");

    html_builder_close_node(&hb, "body");

    html_builder_close_node(&hb, "html");
    response->body.value.string = hb.html_string;

    header_list_append(&response->header_list, header_create("Content-Type", "text/html"));
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
    router_add_matched_route(&router, &user_id_route_matcher, &user_id_route);
    router_add_exact_route(&router, "/", &index_route);
    router_add_route(&router, "/", &not_found_route);

    server_set_router(&server, router);

    server_start(&server);

    server_destroy(&server);

    return 0;
}
