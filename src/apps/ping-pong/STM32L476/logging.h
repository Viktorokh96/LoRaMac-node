/*
 * logging.h
 *
 *	Logging module that uses USART2 for Virtual Com
 *
 *  Created on: 18 мая 2018 г.
 *      Author: viktor96
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#if defined( __FILENAME__ )
 #define Log(level, format, ...)	__log(level, __FILENAME__, __LINE__, format, ##__VA_ARGS__)
#else
 #define Log(level, format, ...)	__log(level, __FILE__, __LINE__, format, ##__VA_ARGS__)
#endif


void __log(int logLevel, char *filename, int lineno, char *fmtstr, ...);
void SetLoggingLevel(int loggingLevel);

#endif /* LOGGING_H_ */
