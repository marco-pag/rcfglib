//
// Zynq partial reconfiguration test code
// Marco Pagani - 2016 - marco.pag<#a#t#>outlook.com
//

#ifndef BMP_READER_H_
#define BMP_READER_H_

#include "stdint.h"

int bmp_reader_load_bmp(char *filename, uint32_t *image, uint32_t *image_len);

#endif /* BMP_READER_H_ */
