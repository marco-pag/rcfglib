/*
 * dev_cfg.h
 *
 *  Created on: 25/gen/2016
 *      Author: marco
 */

#ifndef DEV_CFG_H_
#define DEV_CFG_H_

#include "stdint.h"

int dev_cfg_init();
int dev_cfg_setup_interrupt();

int dev_cfg_transfer_bitfile(const void *bitfile_addr, uint32_t bitfile_len);

#endif /* DEV_CFG_H_ */
