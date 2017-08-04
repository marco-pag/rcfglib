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

#ifndef PROBES_H_
#define PROBES_H_

#include "xparameters_ps.h"

// Zybo PmodJF pins
#define JF1_PIN		13
#define JF2_PIN		10
#define JF3_PIN		11
#define JF4_PIN		12

#define JF7_PIN		0
#define JF8_PIN		9
#define JF9_PIN		14
#define JF10_PIN	15

#define MASK_DATA_0_LSW  (XPS_GPIO_BASEADDR + 0x00000000)

int probes_init();

static inline
void probes_pin_set(uint32_t pin_idx)
{
	uint32_t data;

	data = ~(1 << (pin_idx + 16));

	*( (volatile uint32_t *)MASK_DATA_0_LSW ) = data;
}

static inline
void probes_pin_clear(uint32_t pin_idx)
{
	uint32_t data;

	data = ~(1 << (pin_idx + 16));
	data &= 0xffff0000;

	*( (volatile uint32_t *)MASK_DATA_0_LSW ) = data;
}

#endif /* PROBES_H_ */
