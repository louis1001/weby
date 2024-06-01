#include "router.h"
#include "dynamic_array.h"
#include "request.h"
#include "response.h"
#include "string_stuff.h"

IMPL_LIST_TYPE(RoutePair, route_pair)

void route_match_destroy(RouteMatch *rm) {
    switch(rm->type) {
        case RouteMatchType_Exact:
            string_destroy(&rm->content.exact);
            break;
        case RouteMatchType_Prefix:
            string_destroy(&rm->content.prefix);
            break;
        case RouteMatchType_Function:
            // Has to be a global function anyway.
            break;
    }
}

void route_pair_destroy(RoutePair *rp) {
    route_match_destroy(&rp->match);
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
    RouteMatch rm = {
        .type = RouteMatchType_Prefix,
        .content = {.prefix = string_new(path) }
    };

    RoutePair rp = {
        .match = rm,
        .method = GET,
        .handler = handler
    };

    route_pair_list_append(&router->routes, rp);
}

void router_add_exact_route(Router* router, const char *path, RequestHandlerFn handler) {
    RouteMatch rm = {
        .type = RouteMatchType_Exact,
        .content = { .exact = string_new(path) }
    };

    RoutePair rp = {
        .match = rm,
        .method = GET,
        .handler = handler
    };

    route_pair_list_append(&router->routes, rp);
}

void router_add_matched_route(Router *router, RouteMatcherFn fn, RequestHandlerFn handler) {
    RouteMatch rm = {
        .type = RouteMatchType_Function,
        .content = { .fn = fn }
    };

    RoutePair rp = {
        .match = rm,
        .method = GET,
        .handler = handler
    };

    route_pair_list_append(&router->routes, rp);
}

void router_handle_request(
    Router *router,
    Request *request,
    Response *response
) {
    StringView request_path = string_make_view(&request->path);

    for(usize i = 0; i < router->routes.length; i++) {
        RoutePair *route = &router->routes.data[i];

        bool matches = false;

        switch (route->match.type) {
            case RouteMatchType_Exact: {
                StringView route_path = string_make_view(&route->match.content.exact);

                matches = stringview_compare_str(&route_path, request->path.ptr);
                break;
            }
            case RouteMatchType_Prefix: {
                StringView route_path = string_make_view(&route->match.content.prefix);

                matches = stringview_has_prefix(&request_path, &route_path);
                break;
            }
            case RouteMatchType_Function: {
                RouteMatcherFn fn = route->match.content.fn;

                matches = fn(&request->path_components);
                break;
            }
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
