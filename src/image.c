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

#include "image.h"

const int32_t blur_kernel[5][5] = {
	{1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1},
};
const int32_t blur_kernel_sum = 25;

const int32_t sharp_kernel[5][5] = {
	{-1, -1, -1, -1, -1},
	{-1,  2,  2,  2, -1},
	{-1,  2,  8,  2, -1},
	{-1,  2,  2,  2, -1},
	{-1, -1, -1, -1, -1},
};
const int32_t sharp_kernel_sum = 8;

static inline
uint8_t rgb_to_luma(uint32_t pixel)
{
	return (uint8_t)( (RED(pixel) * 76 + GREEN(pixel) * 150 + BLUE(pixel) * 29 ) >> 8 );
}

void image_int_conv_w(const uint32_t *image_in, uint32_t *image_out,
					const int32_t kernel[][KERNEL_WIDTH], int32_t kernel_sum,
					uint32_t img_width, uint32_t img_height )
{
	int32_t x, y;
	int32_t img_x, img_y;
	int32_t k_x, k_y;
	int32_t red, blue, green;
	int32_t pix_idx;

	for(y = 0; y < img_height; ++y) {
		for(x = 0; x < img_width; ++x) {

			red = 0;
			green = 0;
			blue = 0;

			// Multiply and accumulate each kernel value with corresponding image pixel
			for(k_y = 0; k_y < KERNEL_WIDTH; ++k_y) {

				//img_y = (y - KERNEL_WIDTH / 2 + k_y + img_height) % img_height;
				img_y = (y - KERNEL_WIDTH / 2 + k_y + img_height);
				img_y = img_y >= img_height ?
						img_y - img_height :
						img_y;

				for(k_x = 0; k_x < KERNEL_WIDTH; ++k_x) {

					//img_x = (x - KERNEL_WIDTH / 2 + k_x + img_width) % img_width;
					img_x = (x - KERNEL_WIDTH / 2 + k_x + img_width);
					img_x = img_x >= img_width ?
							img_x - img_width :
							img_x;

					pix_idx = img_y * img_width + img_x;

					red += RED(image_in[pix_idx]) * kernel[k_y][k_x];
					green += GREEN(image_in[pix_idx]) * kernel[k_y][k_x];
					blue += BLUE(image_in[pix_idx]) * kernel[k_y][k_x];
				}
			}

			// Clip pixel color components
			red /= kernel_sum;
			green /= kernel_sum;
			blue /= kernel_sum;

			// Write result pixel
			image_out[y * img_width + x] = PACK_RGB(PIX_CLIP(red), PIX_CLIP(green), PIX_CLIP(blue));
		}
	}
}

static void image_extend_line(uint32_t *image, uint32_t line, uint32_t img_width, uint32_t kernel_width)
{
	int i, j;

	// Extend left edge pixels with the neighbors |XX| << |XX|
	for (i = 0; i < kernel_width / 2; ++i) {
		image[line * img_width + i] = image[line * img_width + kernel_width / 2];
	}

	// Extend right edge pixels with the neighbors |XX| >> |XX|
	for (j = img_width - kernel_width / 2; j < img_width; ++j) {
		image[line * img_width + j] = image[line * img_width + j - kernel_width / 2];
	}
}


void image_int_conv(const uint32_t *image_in, uint32_t *image_out,
					const int32_t kernel[][KERNEL_WIDTH], int32_t kernel_sum,
					uint32_t img_width, uint32_t img_height )
{
	int32_t x, y;
	int32_t img_x, img_y;
	int32_t k_x, k_y;
	int32_t red, blue, green;
	int32_t pix_idx;

	// line
	for(y = 0; y < img_height; y++) {
		// Each pixel in the line
		for(x = KERNEL_WIDTH / 2; x < img_width - KERNEL_WIDTH / 2; ++x) {


			red = 0;
			green = 0;
			blue = 0;

			// Multiply and accumulate each kernel value with corresponding image pixel
			for(k_y = 0; k_y < KERNEL_WIDTH; ++k_y) {

				//img_y = (y - KERNEL_WIDTH / 2 + k_y + img_height) % img_height;
				img_y = (y - KERNEL_WIDTH / 2 + k_y + img_height);
				img_y = img_y >= img_height ?
						img_y - img_height :
						img_y;

				for(k_x = 0; k_x < KERNEL_WIDTH; ++k_x) {

//					//img_x = (x - KERNEL_WIDTH / 2 + k_x + img_width) % img_width;
//					img_x = (x - KERNEL_WIDTH / 2 + k_x + img_width);
//					img_x = img_x >= img_width ?
//							img_x - img_width :
//							img_x;

					img_x = x - KERNEL_WIDTH / 2 + k_x;

					pix_idx = img_y * img_width + img_x;

					red += RED(image_in[pix_idx]) * kernel[k_y][k_x];
					green += GREEN(image_in[pix_idx]) * kernel[k_y][k_x];
					blue += BLUE(image_in[pix_idx]) * kernel[k_y][k_x];
				}
			}

			// Clip pixel color components
			red /= kernel_sum;
			green /= kernel_sum;
			blue /= kernel_sum;

			// Write result pixel
			image_out[y * img_width + x] = PACK_RGB(PIX_CLIP(red), PIX_CLIP(green), PIX_CLIP(blue));
		}

		image_extend_line(image_out, x, img_width, KERNEL_WIDTH);
	}
}

void image_int_sobel_w(	const uint32_t *image_in, uint32_t *image_out,
						uint32_t img_width, uint32_t img_height )
{
	int32_t x, y;
	int32_t img_x, img_y;
	int32_t k_x, k_y;
	int32_t pix_idx;
	int32_t pix_luma;

	int32_t dluma_w = 0;
	int32_t dluma_h = 0;
	int32_t dluma;
	uint8_t luma_out;

	const int32_t kernel_w[3][3] = {
		{-1,	0,		1},
		{-2,	0,		2},
		{-1,	0,		1}
	};

	const int32_t kernel_h[3][3] = {
		{1,		2,		1},
		{0,		0,		0},
		{-1,	-2,		-1}
	};

	for(y = 0; y < img_height; ++y) {
		for(x = 0; x < img_width; ++x) {


			dluma_w = 0;
			dluma_h = 0;

			// Multiply and accumulate each kernel value with corresponding image pixel
			for(k_y = 0; k_y < SOBEL_KERNELS_WIDTH; ++k_y) {

				//img_y = (y - SOBEL_KERNELS_WIDTH / 2 + k_y + img_height) % img_height;
				img_y = (y - SOBEL_KERNELS_WIDTH / 2 + k_y + img_height);
				img_y = img_y >= img_height ?
						img_y - img_height :
						img_y;

				for(k_x = 0; k_x < SOBEL_KERNELS_WIDTH; ++k_x) {

					//img_x = (x - SOBEL_KERNELS_WIDTH / 2 + k_x + img_width) % img_width;
					img_x = (x - SOBEL_KERNELS_WIDTH / 2 + k_x + img_width);
					img_x = img_x >= img_width ?
							img_x - img_width :
							img_x;

					pix_idx = img_y * img_width + img_x;

					// Get current input pixel and calculate luma component
					pix_luma = rgb_to_luma(image_in[pix_idx]);

					// Calculate the approximations of the horizontal(w) and vertical(h) derivatives and accumulate
					dluma_w	+= 	pix_luma * kernel_w[k_y][k_x];
					dluma_h += 	pix_luma * kernel_h[k_y][k_x];
				}
			}

			// Sum derivative components
			dluma = ABS(dluma_w) + ABS(dluma_h);

			// Invert
			luma_out = (uint8_t)(255 - (uint8_t)dluma);

			// Threshold
			if (luma_out > 200) {
				luma_out = 255;

			} else if (luma_out < 100) {
				luma_out = 0;
			}

			// Write result pixel
			image_out[y * img_width + x] = PACK_RGB(luma_out, luma_out, luma_out);
		}
	}
}

void image_int_sobel(	const uint32_t *image_in, uint32_t *image_out,
						uint32_t img_width, uint32_t img_height )
{
	int32_t x, y;
	int32_t img_x, img_y;
	int32_t k_x, k_y;
	int32_t pix_idx;
	int32_t pix_luma;

	int32_t dluma_w = 0;
	int32_t dluma_h = 0;
	int32_t dluma;
	uint8_t luma_out;

	const int32_t kernel_w[3][3] = {
		{-1,	0,		1},
		{-2,	0,		2},
		{-1,	0,		1}
	};

	const int32_t kernel_h[3][3] = {
		{1,		2,		1},
		{0,		0,		0},
		{-1,	-2,		-1}
	};

	for(y = 0; y < img_height; y++) {
		for(x = SOBEL_KERNELS_WIDTH / 2; x < img_width - SOBEL_KERNELS_WIDTH / 2; ++x) {

			dluma_w = 0;
			dluma_h = 0;

			// Multiply and accumulate each kernel value with corresponding image pixel
			for(k_y = 0; k_y < SOBEL_KERNELS_WIDTH; ++k_y) {

				//img_y = (y - SOBEL_KERNELS_WIDTH / 2 + k_y + img_height) % img_height;
				img_y = (y - SOBEL_KERNELS_WIDTH / 2 + k_y + img_height);
				img_y = img_y >= img_height ?
						img_y - img_height :
						img_y;

				for(k_x = 0; k_x < SOBEL_KERNELS_WIDTH; ++k_x) {

					//img_x = (x - SOBEL_KERNELS_WIDTH / 2 + k_x + img_width) % img_width;
//					img_x = (x - SOBEL_KERNELS_WIDTH / 2 + k_x + img_width);
//					img_x = img_x >= img_width ?
//							img_x - img_width :
//							img_x;

					img_x = x - SOBEL_KERNELS_WIDTH / 2 + k_x;

					pix_idx = img_y * img_width + img_x;

					// Get current input pixel and calculate luma component
					pix_luma = rgb_to_luma(image_in[pix_idx]);

					// Calculate the approximations of the horizontal(w) and vertical(h) derivatives and accumulate
					dluma_w	+= 	pix_luma * kernel_w[k_y][k_x];
					dluma_h += 	pix_luma * kernel_h[k_y][k_x];
				}
			}

			// Sum derivative components
			dluma = ABS(dluma_w) + ABS(dluma_h);

			// Invert
			luma_out = (uint8_t)(255 - (uint8_t)dluma);

			// Threshold
			if (luma_out > 200) {
				luma_out = 255;

			} else if (luma_out < 100) {
				luma_out = 0;
			}

			// Write result pixel
			image_out[y * img_width + x] = PACK_RGB(luma_out, luma_out, luma_out);
		}

		image_extend_line(image_out, x, img_width, SOBEL_KERNELS_WIDTH);
	}
}






