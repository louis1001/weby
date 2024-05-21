#include "html_builder.h"
#include "server.h"
#include "string_stuff.h"
#include "request.h"
#include "box.h"
#define WITH_IMPL
#include "core.h"
#undef WITH_IMPL

#include "response.h"

const int PORT = 8001;

const Server server = {0};

void index_route(Server *server, Request *req, Response *res) {
    (void) server;
    (void) req;

    res->version = HTTP1_0;
    res->status_code = 200;
    res->body.type = STRING_RESPONSE;
    res->body.value.string = string_new("Working.");
}

int main(void) {
    Server server = {0};

    server_init(&server, 0x0, PORT);

    server_start(&server);

    server_destroy(&server);
    return 0;
}

/*
void router_ideas(void) {
    Router router = router_new();

    router_add_route(&router, "/", &index_route);

    server_set_router(&router);

    router_destroy(&router);
}
*/
