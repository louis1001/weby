#include <errno.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "core.h"
#include "router.h"
#include "server.h"
#include "string_stuff.h"
#include "response.h"

const usize BUFFER_SIZE = 1024;

void default_static_files_route(Request *req, Response *response) {
    printf("Default file route\n");
    StringView request_path = string_make_view(&req->path);
    String full_path = string_new(".");
    printf("Got path: %s\n", full_path.ptr);
    string_append_sv(&full_path, &request_path);

    FILE *file = fopen(full_path.ptr, "r");

    if (file == NULL) {
        debugf("File `%s` was not found", full_path.ptr);
        response->status_code = 404;
        response->body.type = STRING_RESPONSE;
        response->body.value.string = string_new("404 Not Found");
        return;
    }

    int fd = fileno(file);
    struct stat data;
    fstat(fd, &data);

    if (S_ISDIR(data.st_mode)) {
        response->status_code = 302;
        response->body.type = STRING_RESPONSE;

        String redirect_link = string_new("");
        string_append_string(&redirect_link, &req->path);

        if (full_path.ptr[full_path.length-1] != '/') {
            string_append_char(&redirect_link, '/');
        }

        string_append(&redirect_link, "index.html");

        printf("Redirect link: ");
        string_print(&redirect_link);
        printf("\n");

        String body = string_new("Please follow <a src='");
        string_append_string(&body, &redirect_link);
        string_append(&body, "'>this link</a>");

        header_list_append(&response->header_list, header_create("Location", redirect_link.ptr));

        response->body.value.string = body;
        return;
    }

    const char* mime_type = mime_type_for_file(&request_path);
    header_list_append(&response->header_list, header_create("Content-Type", mime_type));
    response->body.type = FILE_RESPONSE;
    response->body.value.file = file;
}

int server_init(Server *server, u32 addr, u16 port) {
    int sfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sfd < 0) {
        show_error("Error creating socket", errno);
        return FAILURE_STATUS;
    }

    socket_addr serv_addr;

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(addr);
    serv_addr.sin_port = htons(port);

    int optval = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    server->addr = serv_addr;
    server->socket_fd = sfd;
    server->status = Setup;
    Router r = router_new();
    router_add_route(&r, "/", &default_static_files_route);

    server->router = r;

    return 0;
}

char *mime_type_for_file(const StringView *sv) {
    SV_SPLIT(sv, '.', route, ext);

    if (stringview_compare_str(&ext, "html")) {
        return "text/html";
    } else if (stringview_compare_str(&ext, "png")) {
        return "image/x-png";
    } else if (stringview_compare_str(&ext, "ico")) {
        return "image/x-icon";
    } else if (stringview_compare_str(&ext, "svg")) {
        return "image/svg+xml";
    } else if (stringview_compare_str(&ext, "css")) {
        return "text/css";
    }

    return "unknown";
}

int server_send_file(Server *server, Request *req, FILE *file) {
    (void) server;

    // Read file into buffer, sending it to the client
    // stop when file is completely read
    char buffer[BUFFER_SIZE];
    usize bytes_read = 0;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(req->client_fd, buffer, bytes_read, 0);
    }
    return 0;
}

int server_send_response(Server *server, Request *req, Response *res) {
    (void) server;
    String response_str = response_head_to_string(res);

    send(req->client_fd, response_str.ptr, response_str.length, 0);
    send(req->client_fd, "\n", 1, 0);

    printf("Response: ---\n");
    string_print(&response_str);

    switch (res->body.type) {
        case NO_RESPONSE:
            break;
        case STRING_RESPONSE:
            send(req->client_fd, res->body.value.string.ptr, res->body.value.string.length, 0);

            string_print(&res->body.value.string);
            break;
        case FILE_RESPONSE:
            server_send_file(server, req, res->body.value.file);

            printf("<file contents>\n");
            break;
    }

    printf("---\n");

    string_destroy(&response_str);

    return 0;
}

void request_debug_print(Request *req) {
    String meth = http_method_to_str(&req->method);
    printf("Method: ");
    string_print(&meth);
    string_destroy(&meth);
    printf("\n");

    printf("Path: ");
    string_print(&req->path);
    printf("\n");

    if (req->path.length > 0) {
        printf("Query: ");
        string_print(&req->query);
        printf("\n");
    }

    String ver = http_version_to_str(&req->version);
    printf("Http Version: ");
    string_print(&ver);
    string_destroy(&ver);
    printf("\n\n");
}

void server_handle_request(Server *server, int slot) {
    int client_fd = server->client_fds[slot];
    debugf("Accepted connection from client (fd: %d)", client_fd);

    char *buffer = (char *) calloc(BUFFER_SIZE, sizeof(char));

    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);

    debugf("Received %zu", bytes_received);

    if (bytes_received <= 0) {
        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
        free(buffer);
        return; // TODO: Handle early return in fork
    }

    RequestBuilder rb = request_builder_create(buffer, client_fd);
    int result = request_builder_parse(&rb);
    if (result < 0) {
        fprintf(stderr, "Error parsing request\n");
    }

    request_debug_print(&rb.request);

    Response res = response_create();
    router_handle_request(&server->router, &rb.request, &res);

    server_send_response(server, &rb.request, &res);

    response_destroy(&res);

    request_builder_destroy(&rb);
    free(buffer);
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);

    server->client_fds[slot] = -1;
}

int server_start(Server *server) {
    fflush(stdout);

    int bind_result = bind(server->socket_fd, (struct sockaddr*) &server->addr, sizeof(server->addr));
    if (bind_result < 0) {
        show_error("Error binding", errno);

        close(server->socket_fd);

        return FAILURE_STATUS;
    }

    if (listen(server->socket_fd, 10) < 0) {
        show_error("Error listening", errno);
        close(server->socket_fd);
        return FAILURE_STATUS;
    }

    printf("Listening on `http://0.0.0.0:8001`\n");

    signal(SIGCHLD, SIG_IGN);

    int slot = 0;

    loop {
        socket_addr client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        server->client_fds[slot] = accept(server->socket_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (server->client_fds[slot] < 0) {
            show_error("Error accepting connection", errno);
            continue;
        }

        int current_slot = slot;
        slot = (slot + 1) % MAXCON;

        server_handle_request(server, current_slot);

        // pthread_t thread_id;
        // pthread_create(&thread_id, NULL, handle_client, (void*) client_fd);
        // pthread_detach(thread_id);
    }
}

void server_destroy(Server *server) {
    close(server->socket_fd);
}

void server_set_router(Server *server, Router router) {
    router_destroy(&server->router);

    server->router = router;
}
