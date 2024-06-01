#ifndef SLOW_DICT_H
#define SLOW_DICT_H

#include "core.h"
#include "string_stuff.h"

typedef struct {
    String key;
    String value;
} SlowDictPair;

DEFINE_LIST_TYPE(SlowDictPair, slow_dict_pair);

typedef struct {
    SlowDictPairList pairs;
} SlowDict;

void slow_dict_pair_destroy(SlowDictPair *);

void slow_dict_init(SlowDict *);
void slow_dict_insert(SlowDict *, String, String);
String* slow_dict_get(SlowDict *, const char*);
void slow_dict_destroy(SlowDict *);

#endif
