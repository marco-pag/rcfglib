//
// Zynq partial reconfiguration test code
// Marco Pagani - 2016 - marco.pag<#a#t#>outlook.com
//

#ifndef LOGGER_H_
#define LOGGER_H_

#define LOG_SIZE_MB 192

#define STRING_SIZE 256
#define QUEUE_SIZE ((LOG_SIZE_MB * 1024 * 1024) / STRING_SIZE)

//#define QUEUE_SIZE 1024 * 1024 * 6
//#define STRING_SIZE 24

/*-----------------------------------------------------------------*/

#define LOG_LEVEL_MUTE		0
#define LOG_LEVEL_SIMPLE	1
#define LOG_LEVEL_FULL		2
#define LOG_LEVEL_PEDANTIC	3

/*-----------------------------------------------------------------*/

#define LOG_GLOBAL_LEVEL LOG_LEVEL_SIMPLE

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
	int level_ = (level);											\
	char p_string[STRING_SIZE];										\
																	\
		if (level_ <= LOG_GLOBAL_LEVEL) {							\
			sprintf(p_string, message, ##args);						\
																	\
			if (logger) {											\
				xQueueSendToBack((logger)->queue, p_string, 0ul);	\
			}														\
		}															\
} while (0)

int logger_init(Logger *self);

void logger_flush_log(Logger *self);

#endif /* LOGGER_H_ */
