#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "logging.h"
#include "uart.h"

extern Uart_t Uart2;

void Log(char *fmtstr, ...)
{
        va_list ap;
        uint8_t buff[128];

        va_start(ap, fmtstr);
        vsnprintf((char *) buff, 128, fmtstr, ap);
        va_end(ap);

		UartPutBuffer(&Uart2, (uint8_t *) buff, strlen((char *) buff));

}
