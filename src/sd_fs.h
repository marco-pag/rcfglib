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

#ifndef SD_FS_H_
#define SD_FS_H_

#include "stdint.h"

int sd_fs_init();
int sd_fs_load_bitfile(const char *bitfile_name, void *dest_addr, uint32_t bitfile_len);

#endif /* SD_FS_H_ */
