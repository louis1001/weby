#include "html_builder.h"

typedef void (*NodeContents)(HtmlBuilder*);

void html_builder_construct(HtmlBuilder *hb, const char *name, NodeContents contents) {
    html_builder_make_node(hb, name);
    contents(hb);
    html_builder_close_node(hb, name);
}

void make_h1(HtmlBuilder *hb) {
    html_builder_add_attribute(hb, "color", "red");
    html_builder_open_node(hb);

    html_builder_add_text(hb, "Hello, world\n");
}

int main(void) {
    // Server server;

    // server_init(&server, 0x0, PORT);

    // server_start(&server);

    HtmlBuilder hb = html_builder_new();
    html_builder_make_node(&hb, "html");
    html_builder_open_node(&hb);

    html_builder_make_node(&hb, "head");
    html_builder_open_node(&hb);
    html_builder_close_node(&hb, "head");

    html_builder_make_node(&hb, "body");
    html_builder_open_node(&hb);

    html_builder_construct(&hb, "h1", &make_h1);

    html_builder_close_node(&hb, "body");

    html_builder_close_node(&hb, "html");

    string_print(&hb.html_string);

    html_builder_destroy(&hb);
    return 0;
}
