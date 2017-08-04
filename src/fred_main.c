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

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Xilinx includes
#include "xil_printf.h"
#include "xparameters.h"
#include "xstatus.h"

#include "stdlib.h"

#include "sd_fs.h"
#include "dev_cfg.h"
#include "parameters.h"
#include "bmp_reader.h"
#include "display.h"
#include "image.h"
#include "matrix.h"
#include "slots_drv.h"
#include "reconfig_manager.h"
#include "bitstreams.h"
#include "logger.h"
#include "probes.h"

//------------------------- Hardware operations --------------------------------//

#include "hw_op.h"
#include "hw_op_image_conv.h"
#include "hw_op_matrix_mult.h"

//------------------------------------------------------------------------------//

//#define SPEED

//------------------------------------------------------------------------------//

static void log_printer_task(void *pvParameters);

static void blur_task(void *pvParameters);
static void sharp_task(void *pvParameters);
static void sobel_task(void *pvParameters);
static void mult_task(void *pvParameters);


//------------------------------------------------------------------------------//

Logger logger;

//------------------------------------------------------------------------------//

uint32_t m_a_in[MATRIX_WIDTH][MATRIX_WIDTH];
uint32_t m_b_in[MATRIX_WIDTH][MATRIX_WIDTH];
uint32_t m_out[MATRIX_WIDTH][MATRIX_WIDTH];
uint32_t m_out_sw[MATRIX_WIDTH][MATRIX_WIDTH];

//------------------------------------------------------------------------------//

uint32_t image_sea[IMAGE_WIDTH * IMAGE_HEIGHT];
uint32_t image_lenna[IMAGE_WIDTH * IMAGE_HEIGHT];
uint32_t image_city[IMAGE_WIDTH * IMAGE_HEIGHT];

uint32_t image_out1[IMAGE_WIDTH * IMAGE_HEIGHT];
uint32_t image_out2[IMAGE_WIDTH * IMAGE_HEIGHT];
uint32_t image_out3[IMAGE_WIDTH * IMAGE_HEIGHT];

//------------------------------------------------------------------------------//

TickType_t tick_limit;

const uint32_t running_time_minutes = 5;

//------------------------------------------------------------------------------//

static inline
int setup_hw()
{
	// TODO: move
	logger_init(&logger);

	init_exception_sys();

	if (rcfg_manager_init(&logger) != XST_SUCCESS) {
		exit(-1);
	}

	return XST_SUCCESS;
}

void postSetupTickInterrupt()
{
	if (setup_hw() != XST_SUCCESS) {
		exit(-1);
	}
}

static
void init_sdfs()
{
	if (sd_fs_init() != XST_SUCCESS) {
		xil_printf("SD init fail \n");
		exit(-1);
	}
}

static
void load_images()
{
	int retval;
	uint32_t image_len;

	retval = bmp_reader_load_bmp("imgs/sea.bmp", image_sea, &image_len);

	if (retval != XST_SUCCESS) {
		xil_printf("error while reading image file\n");
	}

	retval = bmp_reader_load_bmp("imgs/lenna.bmp", image_lenna, &image_len);

	if (retval != XST_SUCCESS) {
		xil_printf("error while reading image file\n");
	}

	retval = bmp_reader_load_bmp("imgs/city.bmp", image_city, &image_len);

	if (retval != XST_SUCCESS) {
		xil_printf("error while reading image file\n");
	}
}

//------------------------------------------------------------------------------//

int main( void )
{
	xil_printf( "Hello from Freertos\r\n" );

	xTaskCreate(log_printer_task, ( const char * ) "log",	2048, NULL,	tskIDLE_PRIORITY, NULL);

	// RM
	xTaskCreate(mult_task, ( const char * ) "mult",	2048, NULL,	tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(sharp_task, ( const char * ) "sharp",	2048, NULL,	tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(blur_task, ( const char * ) "blur",	2048, NULL,	tskIDLE_PRIORITY + 3, NULL);
	xTaskCreate(sobel_task, ( const char * ) "sobel",	2048, NULL,	tskIDLE_PRIORITY + 4, NULL);

	// Init matrices
	matrix_init(m_a_in, 0);
	matrix_init(m_b_in, 0);

	init_sdfs();
	load_bitstreams();
	load_images();

	probes_init();
	display_init(image_sea);

	tick_limit = 	xTaskGetTickCount() +
					pdMS_TO_TICKS(running_time_minutes * 60 * 1000);

	// Start the tasks and timer running
	vTaskStartScheduler();

	for( ;; );
}

//------------------------------------------------------------------------------//

static void log_printer_task(void *pvParameters)
{
	vTaskDelay(tick_limit + pdMS_TO_TICKS(2000UL));

	while (1) {
		logger_flush_log(&logger);
	}
}

//------------------------------------------------------------------------------//

static void sobel_task(void *pvParameters)
{
	TickType_t last_wake_time;
	const TickType_t period = pdMS_TO_TICKS(100UL);

	const uint32_t pin = JF1_PIN;

	static int job = 0;
	Stopwatch task_watch, hw_watch, sw_watch;

	Hw_Op city_img_sobel_op;

	// Init hardware operation
	hw_op_image_conv_init_sobel(&city_img_sobel_op, "City image sobel");

	// Load parameters
	hw_op_image_conv_set_args(&city_img_sobel_op, image_city, image_out3, NULL);

	// Initialize with current time
	last_wake_time = xTaskGetTickCount();

	// Task body
	while (xTaskGetTickCount() < tick_limit) {

		probes_pin_set(pin);

		stopwatch_start(&task_watch);

		logger_log( &logger, LOG_LEVEL_SIMPLE,
				">>> Task \"%s\" job #%i started\n",
				pcTaskGetTaskName(NULL), job);

		// Load parameters
		hw_op_image_conv_set_args(&city_img_sobel_op, image_city, image_out3, NULL);

		// Execute HW-Task
		hw_watch = rcfg_manager_execute_hw_op(&city_img_sobel_op);

		//display_print_image(image_out3);

#ifdef SPEED
		stopwatch_start(&sw_watch);
		image_int_sobel(image_city, image_out3,
						IMAGE_WIDTH, IMAGE_HEIGHT);
		stopwatch_stop(&sw_watch);

		logger_log( &logger, LOG_LEVEL_SIMPLE,
				">>> Task \"%s\" job #%i end. Hw time: %.5f, SW time: %.5f, speedup: %.5f X\n",
				pcTaskGetTaskName(NULL), job, stopwatch_get_ms(&hw_watch),
				stopwatch_get_ms(&sw_watch), stopwatch_get_ratio(&sw_watch, &hw_watch));
#endif

		//display_print_image(image_out3);

		stopwatch_stop(&task_watch);

		logger_log( &logger, LOG_LEVEL_SIMPLE,
				">>> Task \"%s\" job #%i Response time: %.5f ms\n",
				pcTaskGetTaskName(NULL), job, stopwatch_get_ms(&task_watch));

		// Check for deadline miss
		if (xTaskGetTickCount() > last_wake_time + period) {
			logger_log( &logger, LOG_LEVEL_SIMPLE,
					">>> Task \"%s\" job #%i DEADLINE MISS\n",
					pcTaskGetTaskName(NULL), job);
		}

		++job;

		probes_pin_clear(pin);

		// Wait for the next cycle
		vTaskDelayUntil(&last_wake_time, period);
	}

	// Suspend after time limit
	vTaskSuspend(NULL);
}

static void blur_task(void *pvParameters)
{
	const TickType_t period = pdMS_TO_TICKS(150UL);
	TickType_t last_wake_time;

	const uint32_t pin = JF2_PIN;

	static int job = 0;
	Stopwatch task_watch, hw_watch, sw_watch;

	Hw_Op sea_img_blur_op;

	// Init hardware operation
	hw_op_image_conv_init_blur(&sea_img_blur_op, "Sea image blur");

	// Load parameters
	hw_op_image_conv_set_args(&sea_img_blur_op, image_sea, image_out1, NULL);

	// Initialize with current time
	last_wake_time = xTaskGetTickCount();

	probes_init();

	// Task body
	while (xTaskGetTickCount() < tick_limit) {

		probes_pin_set(pin);

		stopwatch_start(&task_watch);

		logger_log( &logger, LOG_LEVEL_SIMPLE,
				">>> Task \"%s\" job #%i started\n",
				pcTaskGetTaskName(NULL), job);

		// Load parameters
		hw_op_image_conv_set_args(&sea_img_blur_op, image_sea, image_out1, NULL);

		// Execute HW-Task
		hw_watch = rcfg_manager_execute_hw_op(&sea_img_blur_op);

		//display_print_image(image_out1);

#ifdef SPEED
		stopwatch_start(&sw_watch);
		image_int_conv(	image_sea, image_out1,
						blur_kernel, blur_kernel_sum,
						IMAGE_WIDTH, IMAGE_HEIGHT);
		stopwatch_stop(&sw_watch);

		logger_log( &logger, LOG_LEVEL_SIMPLE,
				">>> Task \"%s\" job #%i end. Hw time: %.5f, SW time: %.5f, speedup: %.5f X\n",
				pcTaskGetTaskName(NULL), job, stopwatch_get_ms(&hw_watch),
				stopwatch_get_ms(&sw_watch), stopwatch_get_ratio(&sw_watch, &hw_watch));
#endif

		//display_print_image(image_out1);

		stopwatch_stop(&task_watch);

		logger_log( &logger, LOG_LEVEL_SIMPLE,
				">>> Task \"%s\" job #%i Response time: %.5f ms\n",
				pcTaskGetTaskName(NULL), job, stopwatch_get_ms(&task_watch));

		// Check for deadline miss
		if (xTaskGetTickCount() > last_wake_time + period) {
			logger_log( &logger, LOG_LEVEL_SIMPLE,
					">>> Task \"%s\" job #%i DEADLINE MISS\n",
					pcTaskGetTaskName(NULL), job);
		}

		++job;

		probes_pin_clear(pin);

		// Wait for the next cycle
		vTaskDelayUntil(&last_wake_time, period);
	}

	// Suspend after time limit
	vTaskSuspend(NULL);
}

static void sharp_task(void *pvParameters)
{
	const TickType_t period = pdMS_TO_TICKS(170UL);
	TickType_t last_wake_time;

	const uint32_t pin = JF3_PIN;

	static int job = 0;
	Stopwatch task_watch, hw_watch, sw_watch;

	Hw_Op sea_img_sharp_op;

	// Init hardware operation
	hw_op_image_conv_init_sharp(&sea_img_sharp_op, "Sea image sharp");

	// Load parameters
	hw_op_image_conv_set_args(&sea_img_sharp_op, image_sea, image_out1, NULL);

	// Initialize with current time
	last_wake_time = xTaskGetTickCount();

	// Task body
	while (xTaskGetTickCount() < tick_limit) {

		probes_pin_set(pin);

		stopwatch_start(&task_watch);

		logger_log( &logger, LOG_LEVEL_SIMPLE,
				">>> Task \"%s\" job #%i started\n",
				pcTaskGetTaskName(NULL), job);

		// Load parameters
		hw_op_image_conv_set_args(&sea_img_sharp_op, image_sea, image_out1, NULL);

		// Execute HW-Task
		hw_watch = rcfg_manager_execute_hw_op(&sea_img_sharp_op);

		//display_print_image(image_out2);

#ifdef SPEED

		stopwatch_start(&sw_watch);
		image_int_conv(	image_sea, image_out2,
						sharp_kernel, sharp_kernel_sum,
						IMAGE_WIDTH, IMAGE_HEIGHT);
		stopwatch_stop(&sw_watch);

		logger_log( &logger, LOG_LEVEL_SIMPLE,
				">>> Task \"%s\" job #%i end. Hw time: %.5f, SW time: %.5f, speedup: %.5f X\n",
				pcTaskGetTaskName(NULL), job, stopwatch_get_ms(&hw_watch),
				stopwatch_get_ms(&sw_watch), stopwatch_get_ratio(&sw_watch, &hw_watch));

#endif

		//display_print_image(image_out2);

		stopwatch_stop(&task_watch);

		logger_log( &logger, LOG_LEVEL_SIMPLE,
				">>> Task \"%s\" job #%i Response time: %.5f ms\n",
				pcTaskGetTaskName(NULL), job, stopwatch_get_ms(&task_watch));

		// Check for deadline miss
		if (xTaskGetTickCount() > last_wake_time + period) {
			logger_log( &logger, LOG_LEVEL_SIMPLE,
					">>> Task \"%s\" job #%i DEADLINE MISS\n",
					pcTaskGetTaskName(NULL), job);
		}

		++job;

		probes_pin_clear(pin);

		// Wait for the next cycle
		vTaskDelayUntil(&last_wake_time, period);
	}

	// Suspend after time limit
	vTaskSuspend(NULL);
}

static void mult_task(void *pvParameters)
{
	const TickType_t period = pdMS_TO_TICKS(2500UL);
	TickType_t last_wake_time;

	const uint32_t pin = JF4_PIN;

	static int job = 0;
	Stopwatch task_watch, hw_watch, sw_watch;

	Hw_Op matrix_mult_op;

	// Init hardware operation
	hw_op_matrix_alg_init_mult(&matrix_mult_op, "Matrix multiplication");

	// Load parameters
	hw_op_matrix_alg_set_args(&matrix_mult_op, m_a_in, m_b_in, m_out);

	// Initialize with current time
	last_wake_time = xTaskGetTickCount();

	// Task body
	while (xTaskGetTickCount() < tick_limit) {

		probes_pin_set(pin);

		stopwatch_start(&task_watch);

		logger_log( &logger, LOG_LEVEL_SIMPLE,
				">>> Task \"%s\" job #%i started\n",
				pcTaskGetTaskName(NULL), job);

		// Load parameters
		hw_op_matrix_alg_set_args(&matrix_mult_op, m_a_in, m_b_in, m_out);

		// Execute HW-Task
		hw_watch = rcfg_manager_execute_hw_op(&matrix_mult_op);

#ifdef SPEED

		stopwatch_start(&sw_watch);
		matrix_mult_sw(m_a_in, m_b_in, m_out_sw);
		stopwatch_stop(&sw_watch);

		logger_log( &logger, LOG_LEVEL_SIMPLE,
				">>> Task \"%s\" job #%i end. Hw time: %.5f, SW time: %.5f, speedup: %.5f X\n",
				pcTaskGetTaskName(NULL), job, stopwatch_get_ms(&hw_watch),
				stopwatch_get_ms(&sw_watch), stopwatch_get_ratio(&sw_watch, &hw_watch));

#endif

		stopwatch_stop(&task_watch);

		logger_log( &logger, LOG_LEVEL_SIMPLE,
				">>> Task \"%s\" job #%i Response time: %.5f ms\n",
				pcTaskGetTaskName(NULL), job, stopwatch_get_ms(&task_watch));

		// Check for deadline miss
		if (xTaskGetTickCount() > last_wake_time + period) {
			logger_log( &logger, LOG_LEVEL_SIMPLE,
					">>> Task \"%s\" job #%i DEADLINE MISS\n",
					pcTaskGetTaskName(NULL), job);
		}

		++job;

		probes_pin_clear(pin);

		// Wait for the next cycle
		vTaskDelayUntil(&last_wake_time, period);
	}

	// Suspend after time limit
	vTaskSuspend(NULL);
}



