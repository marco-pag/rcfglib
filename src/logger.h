/*
 * Partial reconfiguration on Zynq test code.
 *
 * Copyright (C) 2016, Marco Pagani, ReTiS Lab.
 * <marco.pag(at)outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef LOGGER_H_
#define LOGGER_H_

#define LOG_SIZE_MB 192

#define STRING_SIZE 256
#define QUEUE_SIZE ((LOG_SIZE_MB * 1024 * 1024) / STRING_SIZE)

/*-----------------------------------------------------------------*/

#define LOG_LEVEL_MUTE		0
#define LOG_LEVEL_SIMPLE	1
#define LOG_LEVEL_FULL		2
#define LOG_LEVEL_PEDANTIC	3

/*-----------------------------------------------------------------*/

#ifndef LOG_GLOBAL_LEVEL
#define LOG_GLOBAL_LEVEL LOG_LEVEL_MUTE
#endif

/*-----------------------------------------------------------------*/


#include "FreeRTOS.h"
#include "stdio.h"
#include "task.h"
#include "queue.h"

// TODO: add time stamp
typedef struct Logger_ {

	QueueHandle_t queue;

} Logger;

#define logger_log(logger, level, message, args...)					\
do {																\
	const int level_ = (level);										\
	char p_string[STRING_SIZE];										\
																	\
		if (level_ <= LOG_GLOBAL_LEVEL) {							\
			sprintf(p_string, (message), ##args);					\
																	\
			if (logger) {											\
				xQueueSendToBack((logger)->queue, p_string, 0ul);	\
			}														\
		}															\
} while (0)

int logger_init(Logger *self);

void logger_flush_log(Logger *self);

#endif /* LOGGER_H_ */
