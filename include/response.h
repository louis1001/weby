#ifndef RESPONSE_H
#define RESPONSE_H
#include "string_stuff.h"
#include "request.h"
#include "core.h"
#include "dynamic_array.h"

typedef struct {
    String name;
    String body;
} Header;

DEFINE_LIST_TYPE(Header, header);

Header header_create(const char*, const char*);
void header_destroy(Header*);

typedef enum {
    NO_RESPONSE,
    STRING_RESPONSE,
    FILE_RESPONSE
} ResponseBodyType;

typedef union {
    String string;
    FILE *file;
} ResponseBodyValue;

typedef struct {
    ResponseBodyType type;
    ResponseBodyValue value;
} ResponseBody;

typedef struct {
    HttpVersion version;
    u16 status_code;
    HeaderList header_list;
    ResponseBody body;
} Response;

Response response_create(void);
void response_destroy(Response*);

void response_body_destroy(ResponseBody*);

String response_head_to_string(Response*);

void status_to_str(u16, String *);

#endif
