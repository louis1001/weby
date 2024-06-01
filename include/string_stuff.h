#ifndef STRINGSTUFF_H
#define STRINGSTUFF_H

#include "dynamic_array.h"
#include "core.h"

typedef struct {
    char *ptr;
    usize length;
    usize capacity;
} String;

typedef struct {
    const char *ptr;
    usize length;
} StringView;

DEFINE_LIST_TYPE(String, string);

StringView stringview_create(const char *);
StringView stringview_create_with_length(const char *, usize);
void stringview_print(const StringView *sv);
void stringview_debug_print(const StringView *sv);
char stringview_char_at(const StringView *sv, usize pos);
void stringview_split(const StringView *sv, char, StringView *lhs, StringView *rhs);
void stringview_split_str(const StringView *, char *, StringView *lhs, StringView *rhs);
bool stringview_compare_str(StringView *, const char*);
bool stringview_compare_sv(StringView *, StringView *);
bool stringview_has_prefix(StringView *, StringView *);

#define SV_SPLIT(sv, separator, lhs_name, rhs_name) StringView lhs_name = {0};\
    StringView rhs_name = {0};\
    stringview_split(sv, separator, &lhs_name, &rhs_name)

#define SV_SPLIT_STR(sv, separator, lhs_name, rhs_name) StringView lhs_name = {0};\
    StringView rhs_name = {0};\
    stringview_split_str(sv, separator, &lhs_name, &rhs_name)

String stringview_to_string(const StringView *sv);
void stringview_triml(StringView *sv, usize);
void stringview_trimr(StringView *sv, usize);
DEFINE_LIST_TYPE(StringView, stringview);

void stringview_list_unsafe_remove_first(StringViewList *);

bool string_compare_str(String *, const char*);

int string_init(String *);
String string_new(const char*);
void string_destroy(String *);
void string_print(String *sv);
int string_append_char(String *, char);
int string_append(String *, const char*);
int string_append_string(String *, String*);
int string_append_sv(String *, const StringView*);
StringView string_substring(const String *, usize start, usize end);
StringViewList string_split_all(const String *str, const char *separator);

StringView string_make_view(const String *);

#endif
