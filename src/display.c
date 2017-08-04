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

#include "display.h"

#include "display_ctrl.h"
#include "xil_cache.h"

#define VGA_BASEADDR XPAR_AXI_DISPCTRL_0_S_AXI_BASEADDR
#define VGA_VDMA_ID XPAR_AXIVDMA_0_DEVICE_ID

#define DISPLAY_HDMI 1
#define DISPLAY_NOT_HDMI 0

#define DISPLAY_MAX_FRAME (1920 * 1080)
#define DISPLAY_STRIDE_BYTE (1920 * 4)


// Display
static DisplayCtrl display_ctrl;

// Frame buffer
uint32_t frame_buffer[DISPLAY_NUM_FRAMES][DISPLAY_MAX_FRAME];

static int next_frame = 0;

static inline
void flush_framebuffer()
{
	Xil_DCacheFlushRange( (uint32_t)display_ctrl.framePtr[next_frame], DISPLAY_MAX_FRAME * sizeof(uint32_t) );
}

int display_init(const uint32_t *image)
{
	int i, status;
	uint32_t *frames[DISPLAY_NUM_FRAMES];

	for (i = 0; i < DISPLAY_NUM_FRAMES; ++i) {
		frames[i] = &frame_buffer[i][0];
	}

	status = DisplayInitialize(&display_ctrl, VGA_VDMA_ID, VGA_BASEADDR, DISPLAY_NOT_HDMI, frames, DISPLAY_STRIDE_BYTE);
	if (status != XST_SUCCESS) {
		xil_printf("Display initialization failed with error code:%d \n", status);
		return XST_FAILURE;
	}

	// Set default mode
	DisplaySetMode(&display_ctrl, &VMODE_800x600);

	status = DisplayStart(&display_ctrl);
	if (status != XST_SUCCESS) {
		xil_printf("Couldn't start display during demo initialization%d\r\n", status);
		return XST_FAILURE;
	}

	if (image) {
		display_print_image(image);
	}

	return XST_SUCCESS;
}

void display_print_image(const uint32_t *image)
{
	uint32_t h, w;
	uint32_t img_line_start, fb_line_start;

	fb_line_start = 0;
	img_line_start = display_ctrl.vMode.width * (display_ctrl.vMode.height - 1);

	// For each line
	for (h = 0; h < display_ctrl.vMode.height; ++h) {

		// For each pixel in the line
		for (w = 0; w < display_ctrl.vMode.width; ++w) {

			// Copy line pixel
			(display_ctrl.framePtr[next_frame])[fb_line_start + w] = image[img_line_start + w];
		}

		img_line_start -= display_ctrl.vMode.width;
		fb_line_start += display_ctrl.stride / sizeof(uint32_t);
	}


	flush_framebuffer();
	DisplayChangeFrame(&display_ctrl, next_frame);

	next_frame = next_frame == 0 ? 1 : 0;
}





