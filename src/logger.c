//
// Zynq partial reconfiguration test code
// Marco Pagani - 2016 - marco.pag<#a#t#>outlook.com
//

#include "logger.h"
#include "xil_printf.h"

//typedef struct Message_ {
//
//	QueueHandle_t queue;
//
//} Message;

int logger_init(Logger *self)
{
	self->queue = xQueueCreate(QUEUE_SIZE, STRING_SIZE);

	if (!self->queue) {
		xil_printf("Logger error\n");
		return -1;
	}

	xil_printf("Logger initialized\n");
	return 0;
}

void logger_flush_log(Logger *self)
{
	char p_string[STRING_SIZE];

	if (!self->queue) {
		return;
	}

	// portMAX_DELAY: Wait without a timeout for data.
	xQueueReceive(self->queue, p_string, portMAX_DELAY);
	xil_printf(p_string);
}
