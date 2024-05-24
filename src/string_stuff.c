#include "string_stuff.h"
#include "core.h"
#include <string.h>
#include <stdio.h>

#define min(a, b) (a <= b ? a : b)
#define max(a, b) (a >= b ? a : b)

StringView stringview_create(const char *str) {
    usize len = strlen(str);

    StringView sv = { .ptr = str, .length = len };

    return sv;
}

StringView stringview_create_with_length(const char *str, usize len) {
    StringView sv = { .ptr = str, .length = len };

    return sv;
}

void stringview_print(StringView *sv) {
    for(usize i = 0; i < sv->length; i++) {
        putc(sv->ptr[i], stdout);
    }
}

void stringview_debug_print(StringView *sv) {
    for(usize i = 0; i < sv->length; i++) {
        char c = sv->ptr[i];
        switch (c) {
        case '\n':
            printf("\\n");
            break;
        case '\r':
            printf("\\r");
            break;
        case '\t':
            printf("\\t");
            break;
        default:
            putc(c, stdout);
            break;
        }
    }
}

char stringview_char_at(StringView *sv, usize pos) {
    if (pos > sv->length) { return '\0'; }

    return sv->ptr[pos];
}

void stringview_split(StringView *sv, char c, StringView *lhs, StringView *rhs) {
    usize found_position = 0;

    while(found_position < sv->length && stringview_char_at(sv, found_position) != c) {
        found_position++;
    }

    lhs->ptr = sv->ptr;
    lhs->length = found_position;

    if(found_position + 1 > sv->length) {
        rhs->ptr = sv->ptr + sv->length;
        rhs->length = 0;
    } else {
        rhs->ptr = sv->ptr + found_position + 1;
        rhs->length = sv->length - found_position - 1;
    }
}

void stringview_split_str(StringView *sv, char *separator, StringView *lhs, StringView *rhs) {
    usize sep_len = strlen(separator);

    if (sv->length == 0) {
        lhs->ptr = nullptr;
        lhs->length = 0;
        rhs->ptr = nullptr;
        rhs->length = 0;
        return;
    }
    usize found_position = 0;
    bool did_find = false;

    for (; found_position < (sv->length - sep_len); found_position++) {
        if(strncmp(separator, sv->ptr+found_position, sep_len) == 0) {
            did_find = true;
            break;
        }
    }

    if(did_find) {
        lhs->ptr = sv->ptr;
        lhs->length = found_position;

        rhs->ptr = sv->ptr + found_position + sep_len;
        rhs->length = sv->length - found_position - sep_len;
    } else {
        lhs->ptr = sv->ptr;
        lhs->length = sv->length;

        rhs->ptr = sv->ptr + sv->length;
        rhs->length = 0;
    }
}

bool stringview_compare_str(StringView *sv, const char *str) {
    usize str_len = strlen(str);
    if (sv->length != str_len) { return false; }

    return strncmp(sv->ptr, str, sv->length) == 0;
}

bool stringview_compare_sv(StringView *sv1, StringView *sv2) {
    if (sv1->length != sv2->length) { return false; }

    return strncmp(sv1->ptr, sv2->ptr, sv1->length) == 0;
}

bool stringview_has_prefix(StringView *sv, StringView *prefix) {
    if (sv->length < prefix->length) { return false; }

    return strncmp(sv->ptr, prefix->ptr, prefix->length) == 0;
}

void stringview_triml(StringView *sv, usize count) {
    usize chars_removed = count;
    if (sv->length < count) {
        chars_removed = count;
    }

    sv->ptr = sv->ptr + chars_removed;
    sv->length = sv->length - chars_removed;
}

void stringview_trimr(StringView *sv, usize count) {
    usize chars_removed = count;
    if (sv->length < count) {
        chars_removed = count;
    }

    sv->length = sv->length - chars_removed;
}

String stringview_to_string(const StringView *sv) {
    String st;
    string_init(&st);

    string_append_sv(&st, sv);

    return st;
}

void string_print(String *sv) {
    printf("%s", sv->ptr);
}

bool string_compare_str(String *sb, const char* str) {
    usize str_len = strlen(str);
    if (sb->length != str_len) { return false; }

    return strncmp(sb->ptr, str, sb->length) == 0;
}

int string_init(String *sb) {
    usize default_capacity = 8;
    sb->capacity = default_capacity;
    sb->length = 0;
    sb->ptr = calloc(default_capacity, sizeof(char));

    if (sb->ptr == NULL) {
        fprintf(stderr, "Could not allocate the string");
        return FAILURE_STATUS;
    }

    return 0;
}

String string_new(const char *initial_value) {
    String sb;
    string_init(&sb);

    string_append(&sb, initial_value);

    return sb;
}

void string_destroy(String *sb) {
    free(sb->ptr);
    sb->capacity = sb->length = 0;
}

int string_grow_to_length(String *sb, usize new_length) {
    if (new_length < sb->capacity) return 0;

    usize new_capacity = sb->capacity;
    do {
        new_capacity = new_capacity * 2;
    } while(new_capacity <= new_length);

    char *new_ptr = calloc(new_capacity, sizeof(char));
    memcpy(new_ptr, sb->ptr, sb->length);

    if (new_ptr == nullptr) {
        return -1;
    }

    sb->capacity = new_capacity;
    sb->ptr = new_ptr;

    return 0;
}

int string_append_char(String *sb, char c) {
    usize new_length = sb->length + 1;

    usize gr = string_grow_to_length(sb, new_length);
    if (gr < 0) {
        return gr;
    }

    *(sb->ptr + sb->length) = c;

    sb->length = new_length;

    return 0;
}

int string_append(String *sb, const char* str) {
    usize added_len = strlen(str);
    usize new_length = sb->length + added_len;
    usize gr = string_grow_to_length(sb, new_length);
    if (gr < 0) {
        return gr;
    }

    char *append_start = sb->ptr + sb->length;

    memcpy(append_start, str, added_len);

    sb->length = new_length;

    return 0;
}

int string_append_string(String *sb, String *added) {
    usize added_len = added->length;
    usize new_length = sb->length + added_len;
    usize gr = string_grow_to_length(sb, new_length);
    if (gr < 0) {
        return gr;
    }

    char *append_start = sb->ptr + sb->length;

    memcpy(append_start, added->ptr, added_len);

    sb->length = new_length;

    return 0;
}

int string_append_sv(String *st, const StringView *sv) {
    if (sv->length == 0) { return 0; }

    usize added_len = sv->length;
    usize new_length = st->length + added_len;

    usize gr = string_grow_to_length(st, new_length);
    if (gr < 0) {
        return gr;
    }

    char *append_start = st->ptr + st->length;

    memcpy(append_start, sv->ptr, added_len);

    st->length = new_length;

    return 0;
}

StringView string_make_view(const String *sb) {
    StringView sv = {.length = sb->length, .ptr = sb->ptr };

    return sv;
}
