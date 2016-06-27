//
// Zynq partial reconfiguration test code
// Marco Pagani - 2016 - marco.pag<#a#t#>outlook.com
//

#include "hw_op_image_conv.h"

void hw_op_image_conv_set_args(Hw_Op *self, const void *img_in, void *img_out, const void *kern)
{
	self->args[0] = (args_t)img_in;
	self->args[1] = (args_t)img_out;
	self->args[2] = (args_t)kern;
}



