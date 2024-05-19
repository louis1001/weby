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
    return 0;
}
