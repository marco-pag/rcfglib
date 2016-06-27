//
// Zynq partial reconfiguration test code
// Marco Pagani - 2016 - marco.pag<#a#t#>outlook.com
//

#include "stdlib.h"
#include "xil_printf.h"
#include "matrix.h"


void matrix_mult_sw(data_t a[][MATRIX_WIDTH], data_t b[][MATRIX_WIDTH], data_t p[][MATRIX_WIDTH])
{
	int i, j, k;

	// Iterate over the rows of the A matrix
	for(i = 0; i < MATRIX_WIDTH; i++) {
		// Iterate over the columns of the B matrix
		for(j = 0; j < MATRIX_WIDTH; j++) {
			// Do the inner product of a row of A and col of B
			p[i][j] = 0;
			for(k = 0; k < MATRIX_WIDTH; k++) {
				p[i][j] += a[i][k] * b[k][j];
			}
		}
	}
}


void matrix_scale_sw(data_t a[][MATRIX_WIDTH], data_t scale, data_t s[][MATRIX_WIDTH])
{
	int i, j;

	// Iterate over the rows of the A matrix
	for(i = 0; i < MATRIX_WIDTH; i++) {
		// Iterate over the columns of the B matrix
		for(j = 0; j < MATRIX_WIDTH; j++) {
			s[i][j] = a[i][j] * scale;
		}
	}
}

void matrix_sum_scalar(data_t a[][MATRIX_WIDTH], data_t scalar)
{
	int i, j;

	// Iterate over the rows of the A matrix
	for(i = 0; i < MATRIX_WIDTH; i++) {
		// Iterate over the columns of the B matrix
		for(j = 0; j < MATRIX_WIDTH; j++) {
			a[i][j] += scalar;
		}
	}
}

void matrix_init(data_t m[][MATRIX_WIDTH], uint32_t mode)
{
	int i, j;

	// Iterate over the rows of the A matrix
	for(i = 0; i < MATRIX_WIDTH; i++) {
		// Iterate over the columns of the B matrix
		for(j = 0; j < MATRIX_WIDTH; j++) {
			switch (mode) {

				case 0:
					m[i][j] =  1;
					break;

				case 1:
					m[i][j] = rand() * 100;
					break;

				default:
					m[i][j] =  1;
					break;
			}
		}
	}
}

int matrix_check_equal(const data_t a[][MATRIX_WIDTH], const data_t b[][MATRIX_WIDTH])
{
	int i, j;

	// Iterate over the rows of the A matrix
	for (i = 0; i < MATRIX_WIDTH; i++) {
		// Iterate over the columns of the B matrix
		for(j = 0; j < MATRIX_WIDTH; j++) {
			if (a[i][j] != b[i][j]) {
				return 0;
			}
		}
	}
	return 1;
}

int matrix_check_identity(const data_t a[][MATRIX_WIDTH], data_t val)
{
	int i, j;

	// Iterate over the rows of the A matrix
	for (i = 0; i < MATRIX_WIDTH; i++) {
		// Iterate over the columns of the B matrix
		for(j = 0; j < MATRIX_WIDTH; j++) {
			if (a[i][j] != val) {
				return 0;
			}
		}
	}
	return 1;
}

void matrix_print(data_t m[][MATRIX_WIDTH], uint32_t len)
{
	int i, j;

	if (len > MATRIX_WIDTH) {
		xil_printf("matrix_print: size error\n");
		return;
	}

	// Iterate over the rows of the A matrix
	for (i = 0; i < len; i++) {
		// Iterate over the columns of the B matrix
		for(j = 0; j < len; j++) {
			xil_printf("%i ", m[i][j]);
		}
		xil_printf("\n");
	}
}

