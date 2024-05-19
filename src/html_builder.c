#include "html_builder.h"
#include "string_stuff.h"

HtmlBuilder html_builder_new(void) {
    HtmlBuilder hb = {0};
    hb.state = HtmlRoot;
    string_init(&hb.html_string);

    string_append(&hb.html_string, "<!DOCTYPE html>\n");

    return hb;
}

void html_builder_destroy(HtmlBuilder *hb) {
    string_destroy(&hb->html_string);
}

void html_builder_make_node(HtmlBuilder *hb, const char *name) {
    // FIXME: Don't hard crash here
    ASSERT(hb->state == HtmlRoot || hb->state == HtmlInNode, "Cannot create a node here");

    string_append_char(&hb->html_string, '<');
    string_append(&hb->html_string, name);
    string_append_char(&hb->html_string, ' ');

    hb->state = HtmlOpeningNode;
}

void html_builder_self_close_node(HtmlBuilder *hb) {
    ASSERT(hb->state == HtmlOpeningNode, "Not in a node to be closed");

    string_append(&hb->html_string, "/>\n");

    if (hb->depth <= 1) {
        hb->depth = 0;
        hb->state = HtmlRoot;
    } else {
        hb->depth -= 1;
        hb->state = HtmlInNode;
    }
}

void html_builder_open_node(HtmlBuilder *hb) {
    ASSERT(hb->state == HtmlOpeningNode, "Can only open a node that's being written");
    string_append_char(&hb->html_string, '>');
    string_append_char(&hb->html_string, '\n');

    hb->depth += 1;

    hb->state = HtmlInNode;
}

void html_builder_close_node(HtmlBuilder *hb, const char* name) {
    ASSERT(hb->state == HtmlInNode, "No node to be closed");

    string_append(&hb->html_string, "</");
    string_append(&hb->html_string, name);
    string_append_char(&hb->html_string, '>');
    string_append_char(&hb->html_string, '\n');

    if (hb->depth <= 1) {
        hb->depth = 0;
        hb->state = HtmlRoot;
    } else {
        hb->depth -= 1;
        hb->state = HtmlInNode;
    }
}

void html_builder_add_empty_attribute(HtmlBuilder *hb, const char *name) {
    ASSERT(hb->state == HtmlOpeningNode, "Can only add attributes to opening node");

    string_append(&hb->html_string, name);
    string_append_char(&hb->html_string, ' ');
}

void html_builder_add_attribute(HtmlBuilder *hb, const char *name, const char *value) {
    ASSERT(hb->state == HtmlOpeningNode, "Can only add attributes to opening node");

    string_append(&hb->html_string, name);
    string_append_char(&hb->html_string, '=');
    string_append_char(&hb->html_string, '"');
    string_append(&hb->html_string, value);
    string_append_char(&hb->html_string, '"');
    string_append_char(&hb->html_string, ' ');
}

void html_builder_add_text(HtmlBuilder *hb, const char *contents) {
    ASSERT(hb->state == HtmlInNode, "Can only write text inside of node");

    string_append(&hb->html_string, contents);
}
