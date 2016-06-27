//
// Zynq partial reconfiguration test code
// Marco Pagani - 2016 - marco.pag<#a#t#>outlook.com
//

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "stdint.h"

#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 600

int display_init(const uint32_t *image);
void display_print_image(const uint32_t *image);


#endif /* DISPLAY_H_ */
