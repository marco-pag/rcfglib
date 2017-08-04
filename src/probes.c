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

#include "xparameters.h"
#include "xgpiops.h"
#include "xstatus.h"

#include "probes.h"

#define GPIO_DEVICE_ID  XPAR_XGPIOPS_0_DEVICE_ID
#define PINS_LEN 8

static XGpioPs gpio_;

int probes_init()
{
	int pin_idx;
	int status;
	XGpioPs_Config *gpio_config;

	gpio_config = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
	status = XGpioPs_CfgInitialize(&gpio_, gpio_config, gpio_config->BaseAddr);

	if (status != XST_SUCCESS) {
	        return XST_FAILURE;
	}

	const uint32_t pins[PINS_LEN] = {
			JF1_PIN,
			JF2_PIN,
			JF3_PIN,
			JF4_PIN,
			JF7_PIN,
			JF8_PIN,
			JF9_PIN,
			JF10_PIN
	};

	for (pin_idx = 0; pin_idx < PINS_LEN; ++pin_idx) {
		XGpioPs_SetDirectionPin(&gpio_, pins[pin_idx], 1);
		XGpioPs_SetOutputEnablePin(&gpio_, pins[pin_idx], 1);
	}

	return XST_SUCCESS;
}
