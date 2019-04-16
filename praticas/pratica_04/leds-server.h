#include <contiki.h>
#include <contiki-net.h>
#include <net/rpl/rpl.h>
#include <dev/leds.h>
#include <stdio.h>

#include "utils.h"

static struct uip_udp_conn *leds_server_conn;

static void leds_udp_handler(void);

/*---------------------------------------------------------------------------*/
PROCESS(leds_udp_server_process, "LEDs Server Process");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(leds_udp_server_process, ev, data)
{
  PROCESS_BEGIN();

  printf("LEDs UDP Server Started\n");

  // Abre porta UDP
  leds_server_conn = udp_new(NULL, UIP_HTONS(CONN_PORT_LEDS), NULL);
  udp_bind(leds_server_conn, UIP_HTONS(CONN_PORT_LEDS));

  while(1)
  {
    PROCESS_YIELD();
    if(ev == tcpip_event)
    {
      leds_udp_handler();
    }
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
static void leds_udp_handler(void)
{
    leds_payload_t payload;
    uip_ipaddr_t src_ipaddr;

    if(uip_newdata()) // Tem dado novo na pilha uIP?
    {
        //copia pacote e origem para variáveis locais antes que chegue outro pacote!
        memcpy(payload.i8, uip_appdata, sizeof(payload));
        udp_get_srcaddr(&src_ipaddr);

        // Imprime dados recebidos
        printf("LEDS,");
        printipv6(&src_ipaddr);
        printf(",%d,%d,", payload.comando.cmd, payload.comando.led);
        printf("\n");
    }


    int succeded = 1;
    switch(payload.comando.led)
    {
        case 0: //verde
            if (payload.comando.cmd == 0) {
                leds_off(LEDS_GREEN);
            } else if (payload.comando.cmd == 1) {
                leds_on(LEDS_GREEN);
            } else {
                leds_toggle(LEDS_GREEN);
            }
            break;

        case 1: //vermelho
            if (payload.comando.cmd == 0) {
                leds_off(LEDS_RED);
            } else if (payload.comando.cmd == 1) {
                leds_on(LEDS_RED);
            } else {
                leds_toggle(LEDS_RED);
            }
            break;

        case 2: //ambos
            if (payload.comando.cmd == 0) {
                leds_off(LEDS_GREEN);
                leds_off(LEDS_RED);
            } else if (payload.comando.cmd == 1) {
                leds_on(LEDS_GREEN);
                leds_on(LEDS_RED);
            } else {
                leds_toggle(LEDS_GREEN);
                leds_toggle(LEDS_RED);
            }
            break;

        default:
            succeded = 0;
    }

    if(!succeded) printf("Bad command!\r\n");

    /* Restore server connection to allow data from any node */
    memset(&leds_server_conn->ripaddr, 0, sizeof(leds_server_conn->ripaddr));

    return;
}
