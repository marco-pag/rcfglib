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

#ifndef HW_OP_MATRIX_MULT_H_
#define HW_OP_MATRIX_MULT_H_

void hw_op_matrix_alg_set_args(Hw_Op *self, const data_t *m_a_in, const data_t *m_b_in,
								data_t *m_p_out);

void hw_op_matrix_alg_init_mult(Hw_Op *self, const char name[]);

#endif /* HW_OP_MATRIX_OP_H_ */
