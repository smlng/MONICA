#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "isrpipe.h"
#include "isrpipe/read_timeout.h"
#include "periph/uart.h"

#include "config.h"
#include "strato_wrapper.h"
#include "strato3.h"

#define ENABLE_DEBUG        (1)
#include "debug.h"


#define MINMEA_COORD_UPSCALE    (10000000L)
#define UART_READLINE_TIMEOUT   (1000000UL)

static uint8_t _rx_buf_mem[STRATO3_UART_BUFSIZE];
isrpipe_t strato3_uart_isrpipe = ISRPIPE_INIT(_rx_buf_mem);
static uart_t strato3_dev;
static char cmd[] = "Ti\n";

/* Code parts are taken from https://github.com/RIOT-OS/RIOT/pull/7086 */
static ssize_t _readline(isrpipe_t *isrpipe, char *resp_buf, size_t len, uint32_t timeout)
{
    DEBUG("%s\n", __func__);
    ssize_t res = -1;
    char *resp_pos = resp_buf;

    tsrb_empty(&isrpipe->tsrb);

    memset(resp_buf, 0, len);

    while (len) {
        int read_res;
        if ((read_res = isrpipe_read_timeout(isrpipe, (uint8_t *)resp_pos, 1, timeout)) == 1) {
            if (*resp_pos == '\r') {
                continue;
            }
            if (*resp_pos == '\n') {
                *resp_pos = '\0';
                res = resp_pos - resp_buf;
                goto out;
            }

            resp_pos += read_res;
            len -= read_res;
        }
        else if (read_res == -ETIMEDOUT) {
            res = -ETIMEDOUT;
            break;
        }
    }

out:
    return res;
}

int strato_read(strato3_data_t *data)
{
    DEBUG("%s\n", __func__);

    char linebuf[STRATO3_LINE_BUFSIZE];

    uart_write(strato3_dev, (uint8_t *)cmd, 3);
    ssize_t res = _readline(&strato3_uart_isrpipe,
                            linebuf, sizeof(linebuf),
                            UART_READLINE_TIMEOUT);
    if (res > 3) {
        DEBUG(".. [%s]\n", linebuf);
        if (strato3_parse(linebuf, data) == 0) {
            return 0;
        }
    }
    DEBUG(".. timeout\n");
    return -ETIMEDOUT;
}

void strato_init(uart_t dev, uint32_t baud)
{
    DEBUG("%s\n", __func__);
    strato3_dev = dev;
    /* Initialize UART */
    uart_init(strato3_dev, baud, (uart_rx_cb_t) isrpipe_write_one, &strato3_uart_isrpipe);
}
