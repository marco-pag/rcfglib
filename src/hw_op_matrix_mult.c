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

#include "string.h"
#include "xil_cache.h"
#include "matrix.h"
#include "bitstreams.h"

#include "hw_op.h"

#define MATRIX_MULT_ID 101

static
void hw_op_matrix_alg_pre_op(const Hw_Op *self)
{
	// Flush data cache for data source buffers
	Xil_DCacheFlushRange(self->args[0], MATRIX_WIDTH * MATRIX_WIDTH * sizeof(data_t));
	Xil_DCacheFlushRange(self->args[1], MATRIX_WIDTH * MATRIX_WIDTH * sizeof(data_t));
}

static
void hw_op_matrix_alg_post_op(const Hw_Op *self)
{
	// Invalidate data destination buffer data cache to force correct PS read
	Xil_DCacheInvalidateRange(self->args[2], MATRIX_WIDTH * MATRIX_WIDTH * sizeof(data_t));
}

// ----------------------------------- Public functions ----------------------------------- //

// TODO: the same as conv ops

void hw_op_matrix_alg_set_args(Hw_Op *self, const data_t *m_a_in, const data_t *m_b_in,
								data_t *m_p_out)
{
	self->args[0] = (uint32_t)m_a_in;
	self->args[1] = (uint32_t)m_b_in;
	self->args[2] = (uint32_t)m_p_out;
}

void hw_op_matrix_alg_init_mult(Hw_Op *self, const char name[])
{
	hw_op_init(self, (name != NULL ? name : "Matrix multiplication"), bits_mult, MATRIX_MULT_ID);
	hw_op_set_ops(self, hw_op_matrix_alg_pre_op, hw_op_matrix_alg_post_op);
}



