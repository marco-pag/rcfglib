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
#include "hw_op_image_conv_480.h"

// data_t and args_t are defined in the slot driver file

// Size in byte, TODO: da sistemare
#define S_IMAGE_SIZE_BYTE (S_IMAGE_WIDTH * S_IMAGE_HEIGHT * sizeof(data_t))
#define KERNEL_SIZE_BYTE (KERNEL_WIDTH * KERNEL_WIDTH * sizeof(data_t))

// TODO: Remove duplication
static
void hw_op_image_conv_pre_op_480(const Hw_Op *self)
{
	// Flush data cache for data source buffers
	Xil_DCacheFlushRange(self->args[0], S_IMAGE_SIZE_BYTE);
}

static
void hw_op_image_conv_post_op_480(const Hw_Op *self)
{
	// Invalidate data destination buffer data cache to force correct PS read
	Xil_DCacheInvalidateRange(self->args[1], S_IMAGE_SIZE_BYTE);
}


// ----------------------------------- Public functions ----------------------------------- //

//TODO: bistreams and hw id should be paired in a module descriptor structure

void hw_op_image_conv_init_sobel_480(Hw_Op *self, const char name[])
{
	hw_op_init(self, (name != NULL ? name : "Image sobel 640x480"), bits_sobel, SOBEL_ID);
	hw_op_set_ops(self, hw_op_image_conv_pre_op_480, hw_op_image_conv_post_op_480);
}
