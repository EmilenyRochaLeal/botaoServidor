#ifndef HTML_RENDERER_H
#define HTML_RENDERER_H

#include "lwip/tcp.h"

void serve_static(const char *url, struct tcp_pcb *tpcb);
void serve_status(struct tcp_pcb *tpcb, int estado);

#endif
