#include "html_renderer.h"
#include <string.h>
#include <stdio.h>

#include "index_html.c"
#include "style_css.c"  

void serve_static(const char *url, struct tcp_pcb *tpcb) {
    if (strcmp(url, "/") == 0 || strcmp(url, "/index.html") == 0) {
        tcp_write(tpcb, index_html, index_html_len, TCP_WRITE_FLAG_COPY);
    } else if (strcmp(url, "/style.css") == 0) {
        tcp_write(tpcb, style_css, style_css_len, TCP_WRITE_FLAG_COPY);
    } else {
        const char *not_found = "HTTP/1.1 404 Not Found\r\n\r\n404 - Not Found";
        tcp_write(tpcb, not_found, strlen(not_found), TCP_WRITE_FLAG_COPY);
    }
}

void serve_status(struct tcp_pcb *tpcb, int estado) {
    char json[128];
    snprintf(json, sizeof(json),
             "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"
             "{\"estado\":\"%s\"}", estado ? "Pressionado" : "Solto");
    tcp_write(tpcb, json, strlen(json), TCP_WRITE_FLAG_COPY);
}
