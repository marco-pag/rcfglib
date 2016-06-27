//
// Zynq partial reconfiguration test code
// Marco Pagani - 2016 - marco.pag<#a#t#>outlook.com
//

#ifndef HW_OP_IMAGE_CONV_H_
#define HW_OP_IMAGE_CONV_H_

#include "hw_op.h"
#include "image.h"

#define KERNEL_SIZE_BYTE (KERNEL_WIDTH * KERNEL_WIDTH * sizeof(data_t))

void hw_op_image_conv_set_args(Hw_Op *self, const void *img_in, void *img_out, const void *kern);

#endif /* HW_OP_IMAGE_CONV_H_ */
