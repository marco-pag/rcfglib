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

#ifndef RECONFIG_MANAGER_H_
#define RECONFIG_MANAGER_H_

#include "stopwatch.h"
#include "logger.h"
#include "hw_op.h"

int rcfg_manager_init(Logger *logger);

Stopwatch rcfg_manager_execute_hw_op(const Hw_Op *hw_op);

#endif /* RECONFIG_MANAGER_H_ */
