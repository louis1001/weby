#include "server.h"
#include "string_stuff.h"
#include "request.h"
#define WITH_IMPL
#include "core.h"
#undef WITH_IMPL

#include "response.h"

const int PORT = 8001;

int main(void) {
    Server server;

    server_init(&server, 0x0, PORT);

    server_start(&server);

    // Response r = response_create();

    // header_list_append(&r.header_list, header_create("Content-Type", "application/json"));
    // header_list_append(&r.header_list, header_create("Access-Control-Allow-Origin", "*"));

    // String content = response_to_string(&r);
    // response_destroy(&r);

    // string_print(&content);

    // string_destroy(&content);

    return 0;
}
