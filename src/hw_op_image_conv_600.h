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

#ifndef HW_OP_IMAGE_CONV_600_H_
#define HW_OP_IMAGE_CONV_600_H_

void hw_op_image_conv_init_blur(Hw_Op *self, const char name[]);

void hw_op_image_conv_init_sharp(Hw_Op *self, const char name[]);

void hw_op_image_conv_init_sobel(Hw_Op *self, const char name[]);

#endif /* HW_OP_IMAGE_CONV_H_ */
