/**
 * @ingroup     climote
 * @{
 *
 * @file
 * @brief       Main control loop
 *
 * @author      smlng <s@mlng.net>
 *
 * @}
 */

// standard
 #include <inttypes.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
// riot
#include "board.h"
#include "fmt.h"
#include "msg.h"
#include "net/af.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"
#include "periph/gpio.h"
#include "xtimer.h"
// own
#include "config.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define COMM_PAN        (0x2121) // lowpan ID
#define COMM_CHAN       (15U)  // channel

extern int coap_init(void);
extern int sensor_init(void);
extern void post_sensordata(char *data, char *path);

size_t node_get_info(char *buf)
{
    DEBUG("[MAIN] %s\n", __func__);
    ipv6_addr_t ipv6_addrs[GNRC_NETIF_IPV6_ADDRS_NUMOF];
    gnrc_netif_t *netif = gnrc_netif_iter(NULL);
    size_t len = 0;
    if (netif != NULL) {
        kernel_pid_t iface = netif->pid;
        int res = gnrc_netapi_get(iface, NETOPT_IPV6_ADDR, 0,
                                  ipv6_addrs, sizeof(ipv6_addrs));
        for (unsigned i = 0; i < (res / sizeof(ipv6_addr_t)); i++) {
            if (ipv6_addr_is_global(&ipv6_addrs[i]) && !ipv6_addr_is_multicast(&ipv6_addrs[i])) {
                char addr_str[IPV6_ADDR_MAX_STR_LEN];
                ipv6_addr_to_str(addr_str, &ipv6_addrs[i], sizeof(addr_str));
                //len += sprintf(buf+len, ", 'addr': '%s'", ipv6_addr_str);
                len = sprintf(buf, "{'addr': '%s'}", addr_str);
                break;
            }
        }
    }
    return len;
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

/**
 * @brief the main programm loop
 *
 * @return non zero on error
 */
int main(void)
{
    // some initial infos
    puts(" MONICA LGV Demo - Environmental Sensors");
    puts("=========================================\n");
    // init 6lowpan interface
    LED0_ON;
    DEBUG(".. init network\n");
    if (comm_init() != 0) {
        return 1;
    }
    // start sensor thread
    DEBUG(".: init sensors\n");
    if (sensor_init()<= 0) {
        return 1;
    }
    // start coap thread
    DEBUG(":: init coap\n");
    if (coap_init() < 0) {
        return 1;
    }
    LED0_OFF;
#if (defined(LED1_ON) && defined(LED2_ON))
    LED1_OFF;
    LED2_OFF;
#endif
    DEBUG("\n");
    while(1) {
        char strbuf[CONFIG_STRBUF_LEN];
        int pos = 0;
        int len = CONFIG_STRBUF_LEN - 1;
        int16_t h, t;
        sensor_get_humidity(&h);
        sensor_get_temperature(&t);
        /* post temperature */
        memset(strbuf, '\0', CONFIG_STRBUF_LEN);
        pos += snprintf(strbuf, len, "{\"result\":");
        pos += fmt_s16_dfp((strbuf + pos), t, -2);
        pos += snprintf((strbuf + pos), (len -  pos),"}");
        printf("-- CoAP POST temperature: %s\n", strbuf);
        post_sensordata(strbuf, CONFIG_PATH_TEMPERATURE);
        /* post humitity */
        memset(strbuf, '\0', CONFIG_STRBUF_LEN);
        pos = 0;
        len = CONFIG_STRBUF_LEN - 1;
        pos += snprintf(strbuf, len, "{\"result\":");
        pos += fmt_s16_dfp((strbuf + pos), h, -2);
        pos += snprintf((strbuf + pos), (len -  pos),"}");
        printf("-- CoAP POST humidity: %s\n", strbuf);
        post_sensordata(strbuf, CONFIG_PATH_HUMITIDY);
        /* wait for next round */
        xtimer_usleep(CONFIG_LOOP_WAIT);
    }
    // should be never reached
    return 0;
}
