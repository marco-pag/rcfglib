//
// Zynq partial reconfiguration test code
// Marco Pagani - 2016 - marco.pag<#a#t#>outlook.com
//

#ifndef DEV_CFG_H_
#define DEV_CFG_H_

#include "stdint.h"

int dev_cfg_init();
int dev_cfg_setup_interrupt();

int dev_cfg_transfer_bitfile(const void *bitfile_addr, uint32_t bitfile_len);

#endif /* DEV_CFG_H_ */
