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

#include "hw_op_image_conv.h"

void hw_op_image_conv_set_args(Hw_Op *self, const void *img_in, void *img_out, const void *kern)
{
	self->args[0] = (args_t)img_in;
	self->args[1] = (args_t)img_out;
	self->args[2] = (args_t)kern;
}



