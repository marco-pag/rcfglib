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

#ifndef BITSTREAMS_H_
#define BITSTREAMS_H_

//TODO: move in separate modules
// Each set of bitstreams is a propriety of the operation

extern const void *bits_blur[NUM_SLOTS];

extern const void *bits_sharp[NUM_SLOTS];

extern const void *bits_sobel[NUM_SLOTS];

extern const void *bits_mult[NUM_SLOTS];

void load_bitstreams();

#endif /* BITSTREAMS_H_ */
