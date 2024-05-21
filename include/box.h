#ifndef BOX_H
#define BOX_H

#include "core.h"

#define DEFINE_BOX(Element, element_lower) typedef struct {\
    Element *value;\
    bool has_value;\
} Element ## Box;\
\
Element ## Box element_lower ## _box_new(Element value);\
void element_lower ## _box_destroy(Element ## Box *box)

#define IMPL_BOX(Element, element_lower)\
Element ## Box element_lower ## _box_new(Element value) {\
    Element *ptr = malloc(sizeof(Element));\
    memset(ptr, 0, sizeof(Element));\
    \
    *ptr = value;\
    \
    Element ## Box box = {0};\
    box.value = ptr;\
    box.has_value = true;\
    return box;\
}\
\
void element_lower ## _box_destroy(Element ## Box *box) {\
    if(box->has_value) {\
        element_lower ## _destroy(box->value);\
        free(box->value);\
    }\
    box->has_value = false;\
}

typedef struct { usize i; } Something;

#endif
