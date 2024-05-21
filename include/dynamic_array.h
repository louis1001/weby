#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H
#include "core.h"

#define DEFINE_LIST_TYPE(element, element_lower) typedef struct {\
    element *data;\
    usize length;\
    usize capacity;\
} element##List;\
\
int element_lower##_list_init(element##List*);\
void element_lower##_list_destroy(element##List*);\
int element_lower##_list_append(element##List*, element);\
int element_lower##_list_grow_to_length(element##List*, usize)

#define IMPL_LIST_TYPE(element, element_lower) int element_lower##_list_init(element##List* list) {\
    usize default_capacity = 8;\
    list->capacity = default_capacity;\
    list->length = 0;\
    list->data = calloc(default_capacity, sizeof(element));\
    if (list->data == NULL) {\
        fprintf(stderr, "Could not allocate the list");\
        return FAILURE_STATUS;\
    }\
    return 0;\
}\
void element_lower##_list_destroy(element##List* list) {\
    for (usize i = 0; i < list->length; i++) {\
        element el = list->data[i];\
        element_lower##_destroy(&el);\
    }\
    free(list->data);\
    list->length = list->capacity = 0;\
}\
int element_lower##_list_append(element##List* list, element item) {\
    usize new_length = list->length + 1;\
    int gr = element_lower##_list_grow_to_length(list, new_length);\
    if (gr < 0) {\
        return gr;\
    }\
    \
    *(list->data + list->length) = item;\
    list->length = new_length;\
    return 0;\
}\
int element_lower##_list_grow_to_length(element##List* list, usize new_length) {\
if (new_length < list->capacity) return 0;\
\
    usize new_capacity = list->capacity;\
    do {\
        new_capacity = new_capacity * 2;\
    } while(new_capacity <= new_length);\
\
    element *new_ptr = calloc(new_capacity, sizeof(element));\
    memcpy(new_ptr, list->data, list->length * sizeof(element));\
\
    if (new_ptr == nullptr) {\
        return -1;\
    }\
\
    list->capacity = new_capacity;\
    list->data = new_ptr;\
\
    return 0;\
}\


#endif
