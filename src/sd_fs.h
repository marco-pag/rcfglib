//
// Zynq partial reconfiguration test code
// Marco Pagani - 2016 - marco.pag<#a#t#>outlook.com
//

#ifndef SD_FS_H_
#define SD_FS_H_

#include "stdint.h"

int sd_fs_init();
int sd_fs_load_bitfile(const char *bitfile_name, void *dest_addr, uint32_t bitfile_len);

#endif /* SD_FS_H_ */
