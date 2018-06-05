#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "logging.h"
#include "uart.h"

extern Uart_t Uart2;

// Default value is 0 (max level)
static int CurrentLogLevel = 0;

void SetLoggingLevel(int loggingLevel)
{
	CurrentLogLevel = loggingLevel;
}

void __log(int logLevel, char *filename, int lineno, char *fmtstr, ...)
{
        va_list ap;
        uint8_t logbuff[192] = {};
        uint8_t buff[128] = {};
		char logfmt[] = "%s:%i:L%i\t";

		if (logLevel > CurrentLogLevel)
			return;

		snprintf((char *) logbuff, 192, logfmt, filename, lineno, logLevel);

        va_start(ap, fmtstr);
        vsnprintf((char *) buff, 128, fmtstr, ap);
        va_end(ap);

		strcat((char *) logbuff, (char *) buff);

		UartPutBuffer(&Uart2, (uint8_t *) logbuff, strlen((char *) logbuff));
}
