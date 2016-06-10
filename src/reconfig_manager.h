/*
 * reconfig_manager.h
 *
 *  Created on: 24/feb/2016
 *      Author: marco
 */

#ifndef RECONFIG_MANAGER_H_
#define RECONFIG_MANAGER_H_

#include "stopwatch.h"
#include "logger.h"
#include "hw_op.h"

int rcfg_manager_init(Logger *logger);

Stopwatch rcfg_manager_execute_hw_op(const Hw_Op *hw_op);

#endif /* RECONFIG_MANAGER_H_ */
