#include "response.h"
#include "dynamic_array.h"

#include <unistd.h>

IMPL_LIST_TYPE(Header, header)

Header header_create(const char *name, const char *body) {
    String name_str;
    string_init(&name_str);
    string_append(&name_str, name);

    String body_str;
    string_init(&body_str);
    string_append(&body_str, body);

    Header h = { .name = name_str, .body = body_str };
    return h;
}

void header_destroy(Header *header) {
    string_destroy(&header->body);
    string_destroy(&header->name);
}

Response response_create(void) {
    Response r;
    r.version = HTTP1_0;
    r.status_code = 200;

    header_list_init(&r.header_list);

    r.body.type = NO_RESPONSE;

    return r;
}

void response_destroy(Response *response) {
    header_list_destroy(&response->header_list);
    response->version = UNKNOWN_VERSION;
    response->status_code = 0;

    response_body_destroy(&response->body);
}

String response_head_to_string(Response *response) {
    String str;
    string_init(&str);

    String version = http_version_to_str(&response->version);
    string_append_string(&str, &version);

    string_append_char(&str, ' ');

    char status_code[5] = {0};
    snprintf(status_code, 5, "%d", response->status_code);
    string_append(&str, status_code);

    string_append_char(&str, ' ');

    String status_text;
    string_init(&status_text);
    status_to_str(response->status_code, &status_text);
    string_append_string(&str, &status_text);
    string_destroy(&status_text);

    string_append(&str, "\n");

    for(usize i = 0; i < response->header_list.length; i++) {
        Header *header = &response->header_list.data[i];
        string_append_string(&str, &header->name);
        string_append_char(&str, ':');
        string_append_char(&str, ' ');

        string_append_string(&str, &header->body);
        string_append(&str, "\n");
    }

    string_destroy(&version);

    return str;
}

void response_body_destroy(ResponseBody *body) {
    switch (body->type) {
        case NO_RESPONSE:
            break;
        case STRING_RESPONSE:
            string_destroy(&body->value.string);
            break;
        case FILE_RESPONSE:
            fclose(body->value.file);
            break;
    }
}

void status_to_str(u16 status_code, String *text) {
    switch (status_code) {
        case 200: {
            string_append(text, "OK");
            break;
        }
        case 302: {
            string_append(text, "Found");
            break;
        }
        case 404: {
            string_append(text, "Not Found");
            break;
        }
        default:
            ASSERT(false, "Unknown status code");
            break;
    }
}
