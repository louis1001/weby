#ifndef STRINGSTUFF_H
#define STRINGSTUFF_H

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

StringView stringview_create(const char *);
StringView stringview_create_with_length(const char *, usize);
void stringview_print(StringView *sv);
void stringview_debug_print(StringView *sv);
char stringview_char_at(StringView *sv, usize pos);
void stringview_split(StringView *sv, char, StringView *lhs, StringView *rhs);
void stringview_split_str(StringView *, char *, StringView *lhs, StringView *rhs);

#define SV_SPLIT(sv, separator, lhs_name, rhs_name) StringView lhs_name = {0};\
    StringView rhs_name = {0};\
    stringview_split(sv, separator, &lhs_name, &rhs_name)

#define SV_SPLIT_STR(sv, separator, lhs_name, rhs_name) StringView lhs_name = {0};\
    StringView rhs_name = {0};\
    stringview_split_str(sv, separator, &lhs_name, &rhs_name)

String stringview_to_string(const StringView *sv);
void stringview_triml(StringView *sv, usize);
void stringview_trimr(StringView *sv, usize);
void string_print(String *sv);

bool string_compare_str(String *, const char*);

int string_init(String *);
String string_new(const char*);
void string_destroy(String *);
int string_append_char(String *, char);
int string_append(String *, const char*);
int string_append_string(String *, String*);
int string_append_sv(String *, const StringView*);

StringView string_make_view(const String *);

#endif
