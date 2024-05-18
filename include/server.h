#ifndef SERVER_H
#define SERVER_H
#include <netinet/in.h>

#include "core.h"
#include "request.h"
#include "response.h"

typedef struct sockaddr_in socket_addr;

typedef enum {
    Uninitialized,
    Setup,
    Started,
    Stopped
} ServerStatus;

typedef struct {
    int socket_fd;
    socket_addr addr;
    ServerStatus status;
} Server;

int server_handle_request(Server*, Request*, Response*);
int server_send_file(Server*, Request*, FILE*);
int server_send_response(Server*, Request*, Response*);
int server_init(Server *server, u32 addr, u16 port);
int server_start(Server *server);

#endif
