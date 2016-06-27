//
// Zynq partial reconfiguration test code
// Marco Pagani - 2016 - marco.pag<#a#t#>outlook.com
//

#ifndef MATRIX_H_
#define MATRIX_H_

#define MATRIX_WIDTH 64

#include "hw_op.h"

void matrix_mult_sw(data_t a[][MATRIX_WIDTH], data_t b[][MATRIX_WIDTH], data_t p[][MATRIX_WIDTH]);

void matrix_scale_sw(data_t a[][MATRIX_WIDTH], data_t scale, data_t s[][MATRIX_WIDTH]);

void matrix_sum_scalar(data_t a[][MATRIX_WIDTH], data_t scalar);

void matrix_init(data_t m[][MATRIX_WIDTH], uint32_t mode);

int matrix_check_equal(const data_t a[][MATRIX_WIDTH], const data_t b[][MATRIX_WIDTH]);

int matrix_check_identity(const data_t a[][MATRIX_WIDTH], data_t val);

void matrix_print(data_t m[][MATRIX_WIDTH], uint32_t len);

#endif /* MATRIX_H_ */
