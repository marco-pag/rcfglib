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

#ifndef SLOTS_DRV_H_
#define SLOTS_DRV_H_

// Configuration switches
#define FREE_RTOS
#define PARTIAL_RECONF

#include "stdint.h"
#include "hw_op.h"
#include "logger.h"

int slots_drv_init_slot(uint32_t slot_idx);

uint32_t slots_drv_get_id(uint32_t slot_idx);

void slots_drv_pre_op(const Hw_Op *hw_op, Logger *logger);

void slots_drv_post_op(const Hw_Op *hw_op, Logger *logger);

void slots_drv_start_op(uint32_t slot_idx, const Hw_Op *hw_op, Logger *logger);

void slots_drv_decouple_slot(uint32_t slot_idx);

void slots_drv_couple_slot(uint32_t slot_idx);

void init_exception_sys();

int slots_drv_setup_interrupts();


#endif /* SLOTS_DRV_H_ */
