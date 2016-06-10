/*
 * hw_op.c
 *
 *  Created on: 20/mag/2016
 *      Author: marco
 */

#include "hw_op.h"
#include "string.h"

void hw_op_init(Hw_Op *self, const char name[], const void *bitstreams[], uint32_t hw_id)
{
	int i;

	strncpy(self->name, name, HW_OP_NAME_LEN);

	self->hw_id = hw_id;

	self->pre_op = 0;
	self->post_op = 0;

	for (i = 0; i < NUM_SLOTS; ++i) {
		// One bitfile for each slot
		self->bitstreams[i] = bitstreams[i];
		// Bitfile size for each slot
		self->bitstreams_len[i] = SLOT_0_BIT_LEN;	//TODO: bitstream structure, different sizes
	}
}

void hw_op_set_ops(Hw_Op *self, void (*pre_op)(const struct Hw_Op_ *), void (*post_op)(const struct Hw_Op_ *))
{
	self->pre_op = pre_op;
	self->post_op = post_op;
}



