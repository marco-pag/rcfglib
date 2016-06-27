//
// Zynq partial reconfiguration test code
// Marco Pagani - 2016 - marco.pag<#a#t#>outlook.com
//

#ifndef HW_OP_MATRIX_MULT_H_
#define HW_OP_MATRIX_MULT_H_

void hw_op_matrix_alg_set_args(Hw_Op *self, const data_t *m_a_in, const data_t *m_b_in,
								data_t *m_p_out);

void hw_op_matrix_alg_init_mult(Hw_Op *self, const char name[]);

#endif /* HW_OP_MATRIX_OP_H_ */
