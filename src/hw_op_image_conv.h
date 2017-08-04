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

#ifndef HW_OP_IMAGE_CONV_H_
#define HW_OP_IMAGE_CONV_H_

#include "hw_op.h"
#include "image.h"

#define SOBEL_ID	303
#define BLUR_ID 	301
#define SHARP_ID	302

#define KERNEL_SIZE_BYTE (KERNEL_WIDTH * KERNEL_WIDTH * sizeof(data_t))

void hw_op_image_conv_set_args(Hw_Op *self, const void *img_in, void *img_out, const void *kern);

#endif /* HW_OP_IMAGE_CONV_H_ */
