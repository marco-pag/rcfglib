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

#include "stdint.h"
#include "display.h"

#ifndef IMAGE_H_
#define IMAGE_H_

#define KERNEL_WIDTH 5
#define SOBEL_KERNELS_WIDTH 3

#define IMAGE_WIDTH DISPLAY_WIDTH
#define IMAGE_HEIGHT DISPLAY_HEIGHT

#define S_IMAGE_WIDTH 640
#define S_IMAGE_HEIGHT 480

#define RED(pixel) ((pixel >> 16) & 0xff)
#define GREEN(pixel) ((pixel >> 8) & 0xff)
#define BLUE(pixel) (pixel & 0xff)

#define ABS(comp) ((comp >= 0) ? comp : -comp)
#define PACK_RGB(r, g, b) ((r << 16) | (g << 8) | (b))
#define PIX_CLIP(pixel) (pixel <= 0 ? 0u : pixel >= 255 ? 255u : pixel)

extern const int32_t blur_kernel[5][5];
extern const int32_t blur_kernel_sum;

extern const int32_t sharp_kernel[5][5];
extern const int32_t sharp_kernel_sum;


void image_int_conv_w(const uint32_t *image_in, uint32_t *image_out,
					const int32_t kernel[][KERNEL_WIDTH], int32_t kernel_sum,
					uint32_t img_width, uint32_t img_height );

void image_int_sobel_w(	const uint32_t *image_in, uint32_t *image_out,
						uint32_t img_width, uint32_t img_height );


void image_int_conv(const uint32_t *image_in, uint32_t *image_out,
					const int32_t kernel[][KERNEL_WIDTH], int32_t kernel_sum,
					uint32_t img_width, uint32_t img_height );

void image_int_sobel(	const uint32_t *image_in, uint32_t *image_out,
						uint32_t img_width, uint32_t img_height );

#endif /* IMAGE_H_ */
