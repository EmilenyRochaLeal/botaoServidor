#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"

#include "utils/wifi-connection/wifi-connection.h"
#include "utils/send-data-to-server/send-data.h"
#include "utils/html-renderer/html_renderer.h"


// Definições de pinos
#define LED_BLUE_PIN 12
#define LED_GREEN_PIN 11
#define LED_RED_PIN 13
#define BUTTON_A 5
#define LED_PIN CYW43_WL_GPIO_LED_PIN

// Debounce simples
int debounce_button() {
    static uint32_t last_time = 0;
    const uint32_t debounce_time = 50;
    if (time_us_32() / 1000 - last_time < debounce_time) return -1;
    last_time = time_us_32() / 1000;
    return !gpio_get(BUTTON_A); // Pull-up ativado
}

// === CALLBACK DE RECEBIMENTO ===
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (!p) {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    char *request = (char *)malloc(p->len + 1);
    memcpy(request, p->payload, p->len);
    request[p->len] = '\0';

    printf("Request: %s\n", request);

    // Controle de LEDs via GET
    if (strstr(request, "GET /blue_on"))  gpio_put(LED_BLUE_PIN, 1);
    if (strstr(request, "GET /blue_off")) gpio_put(LED_BLUE_PIN, 0);
    if (strstr(request, "GET /green_on")) gpio_put(LED_GREEN_PIN, 1);
    if (strstr(request, "GET /green_off"))gpio_put(LED_GREEN_PIN, 0);
    if (strstr(request, "GET /red_on"))   gpio_put(LED_RED_PIN, 1);
    if (strstr(request, "GET /red_off"))  gpio_put(LED_RED_PIN, 0);
    if (strstr(request, "GET /on"))       cyw43_arch_gpio_put(LED_PIN, 1);
    if (strstr(request, "GET /off"))      cyw43_arch_gpio_put(LED_PIN, 0);

    // Leitura da temperatura
    adc_select_input(4);
    uint16_t raw_value = adc_read();
    float temperature = 27.0f - ((raw_value * 3.3f / 4096) - 0.706f) / 0.001721f;

    int button_state = debounce_button();
    const char* button_status = (button_state == 1) ? "Pressionado" : "Solto";

    char html[1024];
    snprintf(html, sizeof(html),
             "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
             "<html><head><meta http-equiv=\"refresh\" content=\"1\">"
             "<title>Controle</title><style>body{text-align:center}</style></head><body>"
             "<h1>LED Control</h1>"
             "<form action=\"./blue_on\"><button>Ligar Azul</button></form>"
             "<form action=\"./blue_off\"><button>Desligar Azul</button></form>"
             "<form action=\"./green_on\"><button>Ligar Verde</button></form>"
             "<form action=\"./green_off\"><button>Desligar Verde</button></form>"
             "<form action=\"./red_on\"><button>Ligar Vermelho</button></form>"
             "<form action=\"./red_off\"><button>Desligar Vermelho</button></form>"
             "<p>Temperatura: %.2f &deg;C</p>"
             "<p>Botão: %s</p>"
             "</body></html>", temperature, button_status);

    tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);

    free(request);
    pbuf_free(p);
    return ERR_OK;
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    tcp_recv(newpcb, tcp_server_recv);
    return ERR_OK;
}

int main() {
    stdio_init_all();
    sleep_ms(3000);
    printf("Iniciando...\n");

    // Inicialização do botão
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    // Inicialização dos LEDs
    gpio_init(LED_BLUE_PIN);  gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN); gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_init(LED_RED_PIN);   gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_put(LED_BLUE_PIN, 0);
    gpio_put(LED_GREEN_PIN, 0);
    gpio_put(LED_RED_PIN, 0);

    // Inicializa Wi-Fi
    init_wifi();

    // Mostra IP
    if (netif_default)
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));

    // Inicia ADC
    adc_init();
    adc_set_temp_sensor_enabled(true);

    // Configura servidor TCP
    struct tcp_pcb *server = tcp_new();
    if (!server || tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao criar/associar servidor TCP\n");
        return -1;
    }

    server = tcp_listen(server);
    tcp_accept(server, tcp_server_accept);
    printf("Servidor ouvindo na porta 80\n");

    absolute_time_t last_send = get_absolute_time();

    while (true) {
        cyw43_arch_poll();

        if (absolute_time_diff_us(last_send, get_absolute_time()) > 1000000) {
            last_send = get_absolute_time();
            int estado = !gpio_get(BUTTON_A);
            printf("Botão A: %s\n", estado ? "Pressionado" : "Solto");

            // Envia para o servidor Node.js externo
            create_request(estado);
        }

        sleep_ms(100);
    }

    cyw43_arch_deinit();
    return 0;
}
