#ifndef _APP_CONFIG_H
#define _APP_CONFIG_H

#include "xtimer.h"

#define COMM_PAN        (0x2424) // lowpan ID
#define COMM_CHAN       (18U)  // channel

#define BLIMP_PROXY_ADDR    "ff02::1"
#define BLIMP_PROXY_PORT    "6583"


#define CONFIG_PROXY_ADDR    "fe80::1ac0:ffee:c0ff:ee42"
#define CONFIG_PROXY_PORT    "5683"


#if 0
/* datastreams for Kappa Future Festival (KFF) MONICA demo */
#define CONFIG_PATH_LOCATION    "/Datastreams(66)/Observations"
#define CONFIG_PATH_HUMITIDY    "/Datastreams(67)/Observations"
#define CONFIG_PATH_PRESSURE    "/Datastreams(68)/Observations"
#define CONFIG_PATH_TEMPERATURE "/Datastreams(69)/Observations"
#else
/* datastreams for Woodstower (WT) MONICA demo */
#define CONFIG_PATH_LOCATION    "/Datastreams(112)/Observations"
#define CONFIG_PATH_HUMITIDY    "/Datastreams(113)/Observations"
#define CONFIG_PATH_PRESSURE    "/Datastreams(114)/Observations"
#define CONFIG_PATH_TEMPERATURE "/Datastreams(115)/Observations"
#endif

/* LoRaWAN application configurations */
#define LORAWAN_BUF_SIZE                (64U)
#define APP_LORAWAN_TX_PORT             1
#define APP_LORAWAN_DATARATE            3
#define APP_LORAWAN_JOIN_RETRY_TIME     10

#define STRATO_UART_BAUDRATE            (57600)
#define STRATO_UART_DEV                 UART_DEV(1)

/* we must respect the duty cycle limitations */
#define APP_SLEEP_TIME_S                (17U)

#endif
