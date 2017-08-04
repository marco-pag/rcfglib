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

#ifndef DEV_CFG_H_
#define DEV_CFG_H_

#include "stdint.h"

int dev_cfg_init();
int dev_cfg_setup_interrupt();

int dev_cfg_transfer_bitfile(const void *bitfile_addr, uint32_t bitfile_len);

#endif /* DEV_CFG_H_ */
