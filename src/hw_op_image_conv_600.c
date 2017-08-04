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

#include "xil_cache.h"
#include "hw_op.h"
#include "bitstreams.h"

#include "image.h"
#include "hw_op_image_conv.h"
#include "hw_op_image_conv_600.h"

// data_t and args_t are defined in the slot driver file

// Size in byte
#define IMAGE_SIZE_BYTE (IMAGE_WIDTH * IMAGE_HEIGHT * sizeof(data_t))
#define KERNEL_SIZE_BYTE (KERNEL_WIDTH * KERNEL_WIDTH * sizeof(data_t))

static
void hw_op_image_conv_pre_op_600(const Hw_Op *self)
{
	// Flush data cache for data source buffers
	Xil_DCacheFlushRange(self->args[0], IMAGE_SIZE_BYTE);

	// If the kernel is specified (for testing purposes)
	if (self->args[2])
		Xil_DCacheFlushRange(self->args[2], KERNEL_SIZE_BYTE);
}

static
void hw_op_image_conv_post_op_600(const Hw_Op *self)
{
	// Invalidate data destination buffer data cache to force correct PS read
	Xil_DCacheInvalidateRange(self->args[1], IMAGE_SIZE_BYTE);
}

// ----------------------------------- Public functions ----------------------------------- //

//TODO: bistreams and hw id should be paired in a module descriptor structure

void hw_op_image_conv_init_blur(Hw_Op *self, const char name[])
{
	hw_op_init(self, (name != NULL ? name : "Image blur 800x600"), bits_blur, BLUR_ID);
	hw_op_set_ops(self, hw_op_image_conv_pre_op_600, hw_op_image_conv_post_op_600);
}

void hw_op_image_conv_init_sharp(Hw_Op *self, const char name[])
{
	hw_op_init(self, (name != NULL ? name : "Image sharp 800x600"), bits_sharp, SHARP_ID);
	hw_op_set_ops(self, hw_op_image_conv_pre_op_600, hw_op_image_conv_post_op_600);
}

void hw_op_image_conv_init_sobel(Hw_Op *self, const char name[])
{
	hw_op_init(self, (name != NULL ? name : "Image sobel 800x600"), bits_sobel, SOBEL_ID);
	hw_op_set_ops(self, hw_op_image_conv_pre_op_600, hw_op_image_conv_post_op_600);
}





