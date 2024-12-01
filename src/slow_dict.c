#include "slow_dict.h"
#include "string.h"
#include "string_stuff.h"

IMPL_LIST_TYPE(SlowDictPair, slow_dict_pair)

void slow_dict_pair_destroy(SlowDictPair *pair) {
    string_destroy(&pair->key);
    string_destroy(&pair->value);
}

void slow_dict_init(SlowDict *dict) {
    slow_dict_pair_list_init(&dict->pairs);
}

void slow_dict_insert(SlowDict *dict, String key, String value) {
    SlowDictPair pair = {.key = key, .value = value};

    SlowDictPair *existing = slow_dict_find(dict, key.ptr);
    if (existing != nullptr) {
        string_destroy(&existing->value);
        existing->value = value;

        return;
    }

    slow_dict_pair_list_append(&dict->pairs, pair);
}

const String* slow_dict_get(SlowDict *dict, const char *str) {
    for(usize i = 0; i < dict->pairs.length; i++) {
        SlowDictPair *pair = &dict->pairs.data[i];
        String *key = &pair->key;

        if(string_compare_str(key, str)) {
            return &pair->value;
        }
    }

    return nullptr;
}

SlowDictPair* slow_dict_find(SlowDict *dict, const char *str) {
    for(usize i = 0; i < dict->pairs.length; i++) {
        SlowDictPair *pair = &dict->pairs.data[i];
        String *key = &pair->key;

        if(string_compare_str(key, str)) {
            return pair;
        }
    }

    return nullptr;
}

void slow_dict_destroy(SlowDict *dict) {
    slow_dict_pair_list_destroy(&dict->pairs);
}
