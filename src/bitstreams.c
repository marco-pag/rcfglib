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
#include "parameters.h"
#include "xstatus.h"
#include "sd_fs.h"
#include "xil_printf.h"

#include "bitstreams.h"


//TODO: move in separate modules
// Each set of bitstreams is a propriety of the operation

// Bitstreams for slot 0
static uint8_t bit_slot_0_blur [SLOT_0_BIT_LEN];
static uint8_t bit_slot_0_sharp [SLOT_0_BIT_LEN];
static uint8_t bit_slot_0_sobel [SLOT_0_BIT_LEN];
static uint8_t bit_slot_0_mult [SLOT_0_BIT_LEN];

// Bitstreams for slot 1
static uint8_t bit_slot_1_blur	[SLOT_1_BIT_LEN];
static uint8_t bit_slot_1_sharp [SLOT_1_BIT_LEN];
static uint8_t bit_slot_1_sobel [SLOT_1_BIT_LEN];
static uint8_t bit_slot_1_mult [SLOT_1_BIT_LEN];

/*-----------------------------------------------------------*/

const void *bits_blur[NUM_SLOTS] =	{
		&bit_slot_0_blur,
		&bit_slot_1_blur
		/* Other bitstreams... */
};

const void *bits_sharp[NUM_SLOTS] =	{
		&bit_slot_0_sharp,
		&bit_slot_1_sharp
		/* Other bitstreams... */
};

const void *bits_sobel[NUM_SLOTS] =	{
		&bit_slot_0_sobel,
		&bit_slot_1_sobel
		/* Other bitstreams... */
};

const void *bits_mult[NUM_SLOTS] =	{
		&bit_slot_0_mult,
		&bit_slot_1_mult
		/* Other bitstreams... */
};


void load_bitstreams()
{
	int retval;

//	bit_mult = malloc(BITSTREAM_LEN);
//	bit_scale = malloc(BITSTREAM_LEN);

//	const char bit1_name[] = "blur.bin";
//	const char bit2_name[] = "sharp.bin";

	const char bit1_name[] = "test";
	const char bit2_name[] = "test";

	retval = sd_fs_load_bitfile("bits/blur_s0.bin", bit_slot_0_blur, SLOT_0_BIT_LEN);
	if (retval != XST_SUCCESS)
		xil_printf("Bitstream %s loading fail \n", bit1_name);

	retval = sd_fs_load_bitfile("bits/blur_s1.bin", bit_slot_1_blur, SLOT_1_BIT_LEN);
	if (retval != XST_SUCCESS)
		xil_printf("Bitstream %s loading fail \n", bit2_name);

	retval = sd_fs_load_bitfile("bits/sharp_s0.bin", bit_slot_0_sharp, SLOT_0_BIT_LEN);
	if (retval != XST_SUCCESS)
		xil_printf("Bitstream %s loading fail \n", bit1_name);

	retval = sd_fs_load_bitfile("bits/sharp_s1.bin", bit_slot_1_sharp, SLOT_1_BIT_LEN);
	if (retval != XST_SUCCESS)
		xil_printf("Bitstream %s loading fail \n", bit2_name);

	retval = sd_fs_load_bitfile("bits/sobel_s0.bin", bit_slot_0_sobel, SLOT_0_BIT_LEN);
	if (retval != XST_SUCCESS)
		xil_printf("Bitstream %s loading fail \n", bit1_name);

	retval = sd_fs_load_bitfile("bits/sobel_s1.bin", bit_slot_1_sobel, SLOT_1_BIT_LEN);
	if (retval != XST_SUCCESS)
		xil_printf("Bitstream %s loading fail \n", bit2_name);

	retval = sd_fs_load_bitfile("bits/mult_s0.bin", bit_slot_0_mult, SLOT_0_BIT_LEN);
	if (retval != XST_SUCCESS)
		xil_printf("Bitstream %s loading fail \n", bit1_name);

	retval = sd_fs_load_bitfile("bits/mult_s1.bin", bit_slot_1_mult, SLOT_1_BIT_LEN);
	if (retval != XST_SUCCESS)
		xil_printf("Bitstream %s loading fail \n", bit2_name);

	xil_printf("All bitstreams successfully loaded to RAM\n");
}
