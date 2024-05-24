#ifndef ROUTER_H
#define ROUTER_H

#include "core.h"
#include "request.h"
#include "response.h"
#include "string_stuff.h"
#include "dynamic_array.h"

typedef void (*RequestHandlerFn)(Request *, Response*);

typedef struct {
    String path;
    bool exact; // Checks exact path or just prefix
    RequestHandlerFn handler;
} RoutePair;

void route_pair_destroy(RoutePair *);

DEFINE_LIST_TYPE(RoutePair, route_pair);

typedef struct {
    RoutePairList routes;
} Router;

Router router_new(void);
void router_destroy(Router *);

void router_add_route(Router*, const char*, RequestHandlerFn);
void router_add_exact_route(Router*, const char*, RequestHandlerFn);

void router_handle_request(Router *, Request *, Response *);

#endif
