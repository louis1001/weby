#include "router.h"
#include "dynamic_array.h"
#include "request.h"
#include "response.h"
#include "string_stuff.h"

IMPL_LIST_TYPE(RoutePair, route_pair)

void route_pair_destroy(RoutePair *rp) {
    string_destroy(&rp->path);
}

Router router_new(void) {
    Router router = {0};

    route_pair_list_init(&router.routes);

    return router;
}

void router_destroy(Router *router) {
    route_pair_list_destroy(&router->routes);
}

void router_add_route(Router* router, const char *path, RequestHandlerFn handler) {
    RoutePair rp = {
        .path = string_new(path),
        .exact = false,
        .handler = handler
    };

    route_pair_list_append(&router->routes, rp);
}

void router_add_exact_route(Router* router, const char *path, RequestHandlerFn handler) {
    RoutePair rp = {
        .path = string_new(path),
        .exact = true,
        .handler = handler
    };

    route_pair_list_append(&router->routes, rp);
}

void router_handle_request(
    Router *router,
    Request *request,
    Response *response
) {
    for(usize i = 0; i < router->routes.length; i++) {
        RoutePair *route = &router->routes.data[i];

        StringView route_path = string_make_view(&route->path);

        bool matches = false;
        if (route->exact) {
            matches = stringview_compare_str(&route_path, request->path.ptr);
        } else {
            StringView request_path = string_make_view(&request->path);
            matches = stringview_has_prefix(&request_path, &route_path);
        }

        if (!matches) { continue; }

        // Handle the request
        (*route->handler)(request, response);

        return;
    }

    // NOT FOUND CASE
    response->status_code = 404;
    response->version = HTTP1_0;
    response->body.type = STRING_RESPONSE;
    response->body.value.string = string_new("Path not found");
}
