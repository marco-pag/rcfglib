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

#ifndef BMP_READER_H_
#define BMP_READER_H_

#include "stdint.h"

int bmp_reader_load_bmp(char *filename, uint32_t *image, uint32_t *image_len);

#endif /* BMP_READER_H_ */
