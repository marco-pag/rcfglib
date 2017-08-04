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

#include "bmp_reader.h"

#include "ff.h"
#include "sd_fs.h"
#include "xstatus.h"
#include "xil_assert.h"
#include "xil_cache.h"
#include "xil_printf.h"

typedef struct __attribute__((packed, aligned(1))) Bitmat_File_Header_ {

	uint16_t bfType;  //specifies the file type
	uint32_t bfSize;  //specifies the size in bytes of the bitmap file
	uint16_t bfReserved1;  //reserved; must be 0
	uint16_t bfReserved2;  //reserved; must be 0
	uint32_t bOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits

} Bitmat_File_Header;

typedef struct __attribute__((packed, aligned(1))) Bitmap_Info_Header_ {

	uint32_t biSize;  //specifies the number of bytes required by the struct
	int32_t biWidth;  //specifies width in pixels
	int32_t biHeight;  //species height in pixels
	uint16_t biPlanes; //specifies the number of color planes, must be 1
	uint16_t biBitCount; //specifies the number of bit per pixel
	uint32_t biCompression;//spcifies the type of compression
	uint32_t biSizeImage;  //size of image in bytes
	int32_t biXPelsPerMeter;  //number of pixels per meter in x axis
	int32_t biYPelsPerMeter;  //number of pixels per meter in y axis
	uint32_t biClrUsed;  //number of colors used by th ebitmap
	uint32_t biClrImportant;  //number of colors that are important

} Bitmap_Info_Header;

static inline
uint32_t rotl32(uint32_t data, uint32_t n)
{
	Xil_AssertNonvoid(n < 32);

	if (!n)
		return data;

	return (data << n) | (data >> (32 - n));
}

int bmp_reader_load_bmp(char *filename, uint32_t *image, uint32_t *image_len)
{
	FIL file;
	FRESULT retval;
	UINT byte_read;
	int i;

    Bitmat_File_Header bitmap_file_header;
    Bitmap_Info_Header bitmap_info_header;

    //open filename in read binary mode
	retval = f_open(&file, filename, FA_READ);
	if (retval != FR_OK) {
		xil_printf(" ERROR : f_open returned %d\r\n", retval);
		return XST_FAILURE;
	}

    //read the bitmap file header
    f_read(&file, (void *)&bitmap_file_header, sizeof(Bitmat_File_Header), &byte_read);

    //verify that this is a bmp file by check bitmap id
    if (bitmap_file_header.bfType !=0x4D42) {
    	f_close(&file);
        return XST_FAILURE;
    }

    //read the bitmap info header
    f_read(&file, (void *)&bitmap_info_header, sizeof(Bitmap_Info_Header), &byte_read);

    //move file point to the begging of bitmap data
    f_lseek(&file, bitmap_file_header.bOffBits);

    //allocate enough memory for the bitmap image data
    *image_len = bitmap_info_header.biSizeImage;
    //*image = malloc(bitmap_info_header.biSizeImage);

    //read in the bitmap image data
    f_read(&file, (void *)image, bitmap_info_header.biSizeImage, &byte_read);

    // Rotate left every word
    for (i = 0; i < bitmap_info_header.biSizeImage / sizeof(uint32_t); ++i) {
    	// BGRA -> ARGB
    	image[i] = rotl32(image[i], 24);
    }

    //close file and return bitmap image data
    f_close(&file);

    return XST_SUCCESS;
}

