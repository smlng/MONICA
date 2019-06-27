#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "fmt.h"
#include "periph/uart.h"
#include "xtimer.h"

#include "net/af.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"
#include "net/gnrc.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/udp.h"

#include "config.h"
#include "strato_wrapper.h"
#include "strato3.h"

#define ENABLE_DEBUG        (1)
#include "debug.h"

#define STRBUF_LEN          (120U)
static char strbuf[STRBUF_LEN];

extern int coap_init(void);
extern void coap_put_data(char *data, char *path);

static void udp_send(char *addr_str, char *port_str, char *data)
{
    int iface;
    uint16_t port;
    ipv6_addr_t addr;

    /* get interface, if available */
    iface = ipv6_addr_split_iface(addr_str);
    if ((iface < 0) && (gnrc_netif_numof() == 1)) {
        iface = gnrc_netif_iter(NULL)->pid;
    }
    /* parse destination address */
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return;
    }
    /* parse port */
    port = atoi(port_str);
    if (port == 0) {
        puts("Error: unable to parse destination port");
        return;
    }

    gnrc_pktsnip_t *payload, *udp, *ip;
    unsigned payload_size;
    /* allocate payload */
    payload = gnrc_pktbuf_add(NULL, data, strlen(data), GNRC_NETTYPE_UNDEF);
    if (payload == NULL) {
        puts("Error: unable to copy data to packet buffer");
        return;
    }
    /* store size for output */
    payload_size = (unsigned)payload->size;
    /* allocate UDP header, set source port := destination port */
    udp = gnrc_udp_hdr_build(payload, port, port);
    if (udp == NULL) {
        puts("Error: unable to allocate UDP header");
        gnrc_pktbuf_release(payload);
        return;
    }
    /* allocate IPv6 header */
    ip = gnrc_ipv6_hdr_build(udp, NULL, &addr);
    if (ip == NULL) {
        puts("Error: unable to allocate IPv6 header");
        gnrc_pktbuf_release(udp);
        return;
    }
    /* add netif header, if interface was given */
    if (iface > 0) {
        gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, NULL, 0);

        ((gnrc_netif_hdr_t *)netif->data)->if_pid = (kernel_pid_t)iface;
        LL_PREPEND(ip, netif);
    }
    /* send packet */
    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, ip)) {
        puts("Error: unable to locate UDP thread");
        gnrc_pktbuf_release(ip);
        return;
    }
    /* access to `payload` was implicitly given up with the send operation above
     * => use temporary variable for output */
    printf("Success: sent %u byte(s) to [%s]:%u\n", payload_size, addr_str,
           port);
}

static void strato_print(const strato3_data_t *data)
{
    printf("strato3_data = {\n");
    printf("  uptime:           %02u:%02u:%02u\n", data->uptime.hour, data->uptime.min, data->uptime.sec);
    printf("  time:             %02u:%02u:%02u\n", data->time.hour, data->time.min, data->time.sec);
    printf("  date:             %u-%02u-%02u\n", data->date.year, data->date.month, data->date.day);
    printf("  valid:            %c\n", data->valid ? 'Y' : 'N');
    printf("  satellites:       %u\n", data->satellites);
    print_str("  latitude:         "); print_float(strato3_tocoord(&data->latitude), 7); puts("");
    print_str("  longitude:        "); print_float(strato3_tocoord(&data->longitude), 7); puts("");
    print_str("  speed (knt):      "); print_float(strato3_tofloat(&data->speed_knt), 4); puts("");
    print_str("  speed (kph):      "); print_float(strato3_tofloat(&data->speed_kph), 1); puts("");
    print_str("  course:           "); print_float(strato3_tofloat(&data->course), 1); puts("");
    print_str("  altitude:         "); print_float(strato3_tofloat(&data->altitude), 1); puts("");
    print_str("  temperature(in):  "); print_float(strato3_tofloat(&data->temperature_board), 1); puts("");
    print_str("  temperature(ex):  "); print_float(strato3_tofloat(&data->temperature), 1); puts("");
    print_str("  humidity:         "); print_float(strato3_tofloat(&data->humidity), 1); puts("");
    print_str("  pressure:         "); print_float(strato3_tofloat(&data->pressure), 3); puts("");
    print_str("  voltage:          "); print_float(strato3_tofloat(&data->voltage), 1); puts("");
    printf("  state:            %d\n", (int)data->state);
    printf("}\n");
}

static int strato_json(const strato3_data_t *data, char *buf, size_t len)
{
    if (!buf || !len) {
        DEBUG("%s: invalid buffer!\n", __func__);
        return (-1);
    }
    memset(buf, '\0', len);
    int pos = 0;
    char fmtbuf[32];
    pos += snprintf(buf, len, "{\'dt\':\'%u-%02u-%02uT%02u:%02u:%02uZ\',",
                    data->date.year, data->date.month, data->date.day,
                    data->time.hour, data->time.min, data->time.sec);
    memset(fmtbuf, '\0', 32);
    fmt_float(fmtbuf, strato3_tocoord(&data->latitude), 7);
    pos += snprintf(buf+pos, len-pos, "\'lat\':%s,", fmtbuf);
    memset(fmtbuf, '\0', 32);
    fmt_float(fmtbuf, strato3_tocoord(&data->longitude), 7);
    pos += snprintf(buf+pos, len-pos, "\'lon\':%s,", fmtbuf);
    memset(fmtbuf, '\0', 32);
    fmt_float(fmtbuf, strato3_tofloat(&data->altitude), 1);
    pos += snprintf(buf+pos, len-pos, "\'alt\':%s,", fmtbuf);
    memset(fmtbuf, '\0', 32);
    fmt_float(fmtbuf, strato3_tofloat(&data->temperature), 1);
    pos += snprintf(buf+pos, len-pos, "\'t\':%s,", fmtbuf);
    memset(fmtbuf, '\0', 32);
    fmt_float(fmtbuf, strato3_tofloat(&data->humidity), 1);
    pos += snprintf(buf+pos, len-pos, "\'h\':%s,", fmtbuf);
    memset(fmtbuf, '\0', 32);
    fmt_float(fmtbuf, strato3_tofloat(&data->pressure), 3);
    pos += snprintf(buf+pos, len-pos, "\'p\':%s}", fmtbuf);
    buf[pos] = '\0';
    return 0;
}

static int comm_init(void)
{
    DEBUG("[MAIN] %s\n", __func__);
    uint16_t pan = COMM_PAN;
    uint16_t chan = COMM_CHAN;
    /* get the PID of the first radio */
    gnrc_netif_t *netif = gnrc_netif_iter(NULL);
    if (netif == NULL) {
        DEBUG("[MAIN] ERROR: no network interface found!\n");
        return 1;
    }
    kernel_pid_t iface = netif->pid;
    /* initialize the radio */
    gnrc_netapi_set(iface, NETOPT_NID, 0, &pan, 2);
    gnrc_netapi_set(iface, NETOPT_CHANNEL, 0, &chan, 2);
    return 0;
}

int main(void)
{
    // some initial infos
    puts(" MONICA Blimp Demo \n");
    puts("===================\n");
    // init 6lowpan interface
    LED0_ON;
    DEBUG(".. init network\n");
    if (comm_init() != 0) {
        return 1;
    }
    DEBUG(".. init sensor\n");
    /* Initialize and enable gps */
    strato_init(STRATO_UART_DEV, STRATO_UART_BAUDRATE);
    DEBUG(".. init coap\n");
    /* Initialize and enable gps */
    if (coap_init() != 0) {
        return 1;
    }

    DEBUG(".. init DONE!\n");
    while (1) {
        strato3_data_t data;
        memset(&data, 0, sizeof(strato3_data_t));
        if(strato_read(&data) == 0) {
            strato_print(&data);
            strato_json(&data, strbuf, STRBUF_LEN);
            printf("%s\n", strbuf);
            udp_send(BLIMP_PROXY_ADDR, BLIMP_PROXY_PORT, strbuf);
        }
        else {
            DEBUG(".. got error\n");
        }
        xtimer_sleep(APP_SLEEP_TIME_S);
    }

    return 0;
}
