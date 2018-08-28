#ifndef CONFIG_H
#define CONFIG_H

#include "xtimer.h"

#define CONFIG_PROXY_ADDR_ULA       "fd16:abcd:ef21:3::1"
#define CONFIG_PROXY_ADDR_LL        "fe80::1ac0:ffee:c0ff:ee21"
#define CONFIG_PROXY_ADDR           CONFIG_PROXY_ADDR_LL
#define CONFIG_PROXY_PORT           "5683"

#define CONFIG_PATH_TEMPERATURE     "/Datastreams(2)/Observations"
#define CONFIG_PATH_HUMITIDY        "/Datastreams(3)/Observations"
#define CONFIG_LOOP_WAIT            (10 * US_PER_SEC)
#define CONFIG_STRBUF_LEN           (32U)

#define SENSOR_TIMEOUT_MS           (5 * US_PER_SEC)
#define SENSOR_NUM_SAMPLES          (10U)
#define SENSOR_THREAD_STACKSIZE     (2 * THREAD_STACKSIZE_DEFAULT)

void sensor_get_temperature(int16_t *t);
void sensor_get_humidity(int16_t *t);

size_t node_get_info(char *buf);

#endif /* CONFIG_H */
