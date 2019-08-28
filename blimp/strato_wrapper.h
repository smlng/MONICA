#include <stdint.h>

#include "periph/uart.h"
#include "strato3.h"

#define STRATO3_LINE_BUFSIZE    (128U)
#define STRATO3_UART_BUFSIZE    (512U)
#define STRATO3_UART_TIMEOUT    (1000000UL)

int strato_read(strato3_data_t *data);

void strato_init(uart_t dev, uint32_t baud);
