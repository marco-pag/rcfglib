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

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "stdint.h"

#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 600

int display_init(const uint32_t *image);
void display_print_image(const uint32_t *image);


#endif /* DISPLAY_H_ */
