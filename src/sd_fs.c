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

#include "ff.h"
#include "sd_fs.h"
#include "xparameters.h"
#include "xsdps.h"
#include "xil_cache.h"
#include "xstatus.h"
#include "xplatform_info.h"

static FATFS fatfs;

int sd_fs_init()
{
	FRESULT retval;
	TCHAR path[] = "0:/";

	retval = f_mount(&fatfs, path, 0);
	if (retval != FR_OK) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int sd_fs_load_bitfile(const char *bitfile_name, void *dest_addr, uint32_t bitfile_len)
{
	FIL file;
	FRESULT retval;
	UINT byte_read;

	uint32_t fsize;

	retval = f_open(&file, bitfile_name, FA_READ);
	if (retval != FR_OK) {
		xil_printf(" ERROR : f_open returned %d\r\n", retval);
		return XST_FAILURE;
	}

	fsize = file_size(&file);
	xil_printf("Bitstream file size: %u, ", fsize);

	retval = f_lseek(&file, 0);
	if (retval != FR_OK) {
		xil_printf(" ERROR : f_lseek returned %d\r\n", retval);
		return XST_FAILURE;
	}

	retval = f_read(&file, (void *)dest_addr, bitfile_len, &byte_read);
	if (retval != FR_OK) {
		xil_printf(" ERROR : f_read returned %d\r\n", retval);
		return XST_FAILURE;
	}

	retval = f_close(&file);
	if (retval != FR_OK) {
		xil_printf(" ERROR : f_close returned %d\r\n", retval);
		return XST_FAILURE;
	}

	xil_printf("Byte reads: %d\r\n", byte_read);

	Xil_DCacheFlush();

	return XST_SUCCESS;
}
