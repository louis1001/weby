#ifndef HTML_BUILDER_H
#define HTML_BUILDER_H
#include "string_stuff.h"

typedef enum {
    HtmlRoot,
    HtmlOpeningNode,
    HtmlInNode,
} HtmlBuiderState;

typedef struct {
    String html_string;
    HtmlBuiderState state;
    usize depth;
} HtmlBuilder;

HtmlBuilder html_builder_new(void);
void html_builder_destroy(HtmlBuilder *);

void html_builder_make_node(HtmlBuilder *, const char*);
void html_builder_self_close_node(HtmlBuilder *);
void html_builder_open_node(HtmlBuilder *);
// FIXME: A stack to keep track of the currently openned node?
//  I don't like having to rely on the user providing the correct node to close.
void html_builder_close_node(HtmlBuilder *, const char*);

void html_builder_add_empty_attribute(HtmlBuilder *, const char*);
void html_builder_add_attribute(HtmlBuilder *, const char*, const char*);

void html_builder_add_text(HtmlBuilder *, const char*);

#endif
