
#ifndef HW_OP_H_
#define HW_OP_H_

#include "stdint.h"
#include "parameters.h"

// TODO: move in slot_drv.h ?
#define HW_OP_ARGS_SIZE 8
#define HW_OP_NAME_LEN	256

typedef uint32_t data_t;
typedef uint32_t args_t;

typedef struct Hw_Op_ {

	// Hardware operation ID
	// Should match the module ID exported by the hardware module
	uint32_t hw_id;

	// Hardware operation bitstreams
	const void *bitstreams[NUM_SLOTS];
	uint32_t bitstreams_len[NUM_SLOTS];

	// Hardware operation Arguments
	args_t args[HW_OP_ARGS_SIZE];

	// Hardware operation Name
	char name[HW_OP_NAME_LEN];

	// ~Private~
	void (* pre_op)(const struct Hw_Op_ *self);
	void (* post_op)(const struct Hw_Op_ *self);

} Hw_Op;


void hw_op_init(Hw_Op *self, const char name[], const void *bitstreams[], uint32_t hw_id);

void hw_op_set_ops(Hw_Op *self, void (*pre_op)(const struct Hw_Op_ *), void (*post_op)(const struct Hw_Op_ *));

#endif /* HW_OP_H_ */
