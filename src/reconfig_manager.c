//
// Zynq partial reconfiguration test code
// Marco Pagani - 2016 - marco.pag<#a#t#>outlook.com
//

#include "xstatus.h"
#include "xil_printf.h"
#include "xtime_l.h"

#include "FreeRTOS.h"
#include "semphr.h"		// Free RTOS semaphores
#include "task.h"		// Free RTOS tasks (for notifications)

#include "stopwatch.h"
#include "probes.h"
#include "dev_cfg.h"
#include "slots_drv.h"
#include "reconfig_manager.h"

// ----------------------------------- Private Variables  ----------------------------------- //

// Slots status array. Since is a shared data structure must
// be accessed is mutual exclusion
static uint32_t slots_busy[NUM_SLOTS] = { 0 };

// Slots status array / Configuration port Mutex
// <<Mutexes include a priority inheritance mechanism>> (FreeRTOS man)
static SemaphoreHandle_t slots_mutex;

// Slots counter semaphore
static SemaphoreHandle_t slots_sem;

Logger *logger_ = NULL;

// ----------------------------------- Private functions ----------------------------------- //

static inline
int take_slot_(const Hw_Op *hw_op)
{
	uint32_t idx = 0;
	uint32_t slot_idx = 0;
	uint32_t rcfg = 1;

	Stopwatch watch;

	logger_log(	logger_, LOG_LEVEL_FULL,
			"Rcfg_Manager: Hardware operation: \"%s\". enqueued by task: \"%s\"\n",
			hw_op->name, pcTaskGetTaskName(NULL));

	stopwatch_start(&watch);

	// Try to take a slot. Wait on the counter semaphore if there isn't one available
	xSemaphoreTake(slots_sem, portMAX_DELAY);

	// Enter the "slot status array / configuration port" critical section
	xSemaphoreTake(slots_mutex, portMAX_DELAY);

	// Search if one of the free slots already contains the require hardware module
	for (idx = 0; idx < NUM_SLOTS; ++idx) {

		// If the slot is not busy
		if (slots_busy[idx] == 0) {
			slot_idx = idx;

			// And contains the required module
			if (slots_drv_get_id(idx) == hw_op->hw_id) {
				rcfg = 0;
				break;
			}
		}
	}

	// Take the slot
	slots_busy[slot_idx] = 1;

	// If the slot already contains the require hardware module
	if (rcfg == 0) {

		// Exit the "slot assignment / configuration port" critical section
		xSemaphoreGive(slots_mutex);

		stopwatch_stop(&watch);

		logger_log(	logger_, LOG_LEVEL_FULL,
				"Rcfg_Manager: Slot %u assigned to Hardware operation: \"%s\", "
				"reserved by task: \"%s\" waiting time: %.5f ms. "
				"Slot already configured for the required operation! Skip reconfiguration\n",
				slot_idx, hw_op->name, pcTaskGetTaskName(NULL), stopwatch_get_ms(&watch));

	// If none of the free slots contains the required module the device must be reconfigured
	} else {

		stopwatch_stop(&watch);

		logger_log(	logger_, LOG_LEVEL_FULL,
				"Rcfg_Manager: Slot %u assigned to Hardware operation: "
				"\"%s\", reserved by task: \"%s\" "
				"waiting time: %.5f ms. Begin reconfiguration...\n",
				slot_idx, hw_op->name, pcTaskGetTaskName(NULL), stopwatch_get_ms(&watch));

		stopwatch_start(&watch);

		// Decouple slot before reconfiguration
		slots_drv_decouple_slot(slot_idx);

		probes_pin_set(JF7_PIN);

		// Begin device reconfiguration (with the slot correspondent bistream)
		// The calling task will be suspended until the PL has been reconfigured
		dev_cfg_transfer_bitfile(hw_op->bitstreams[slot_idx], hw_op->bitstreams_len[slot_idx]);

		probes_pin_clear(JF7_PIN);

		// Re attach slot before reconfiguration
		slots_drv_couple_slot(slot_idx);

		stopwatch_stop(&watch);

		// Exit the "slot assignment / configuration port" critical section
		xSemaphoreGive(slots_mutex);

		logger_log(	logger_, LOG_LEVEL_SIMPLE,
				"Rcfg_Manager: Reconfiguration of slot %u completed in %.5f ms\n",
				slot_idx, stopwatch_get_ms(&watch));
	}

	return slot_idx;
}

static inline
void free_slot(int slot_idx, const Hw_Op *hw_op)
{
	// Enter the "slot status array" critical section
	//xSemaphoreTake(slots_mutex, portMAX_DELAY);

	// Mark the slot as free
	slots_busy[slot_idx] = 0;

	// Enter the "slot status array" critical section
	//xSemaphoreGive(slots_mutex);

	// Signal on the slot counter semaphore
	xSemaphoreGive(slots_sem);

	logger_log(	logger_, LOG_LEVEL_FULL,
			"Rcfg_Manager: Slot %u released by hardware operation: "
			"\"%s\", reserved by task: \"%s\"\n",
			slot_idx, hw_op->name, pcTaskGetTaskName(NULL));
}

// ----------------------------------- Public functions ----------------------------------- //

int rcfg_manager_init(Logger *logger)
{
	int slot_idx;

	logger_ = logger;

	// Setup "default" Hardware modules inside the slots
	if (slots_drv_setup_interrupts() != XST_SUCCESS) {
		xil_printf("Hardware Slots interrupts setup failed\n");
		return XST_FAILURE;
	}

	//TEST
	// Init "default" Hardware modules inside the slots
	for (slot_idx = 0; slot_idx < NUM_SLOTS; ++slot_idx) {

		if (slots_drv_init_slot(slot_idx) == XST_SUCCESS) {
			xil_printf("Hardware slot %i setup successful\n", slot_idx);

		} else {
			xil_printf("Hardware slot %i setup failed\n", slot_idx);
			return XST_FAILURE;
		}
	}

	xil_printf("Rcfg_Manager: Slots setup completed\n");

	// Init device configuration port
	if (dev_cfg_init() != XST_SUCCESS) {
		print("Rcfg_Manager: DevC setup failed\n");
		return XST_FAILURE;
	}

	if (dev_cfg_setup_interrupt() != XST_SUCCESS) {
		print("Rcfg_Manager: DevC interrupt setup failed\n");
		return XST_FAILURE;
	}

	xil_printf("Rcfg_Manager: DevC setup completed\n");

	// Init software
	slots_mutex = xSemaphoreCreateMutex();	// Region mutex
	if (slots_mutex == NULL) {
		xil_printf("Rcfg_Manager: Error while creating mutex\n");
	}

	// Init the counting semaphore to the maximum number of free slots.
	// The second parameter is the maximum value the semaphore can reach
	// which is equally to the number of free slots
	slots_sem = xSemaphoreCreateCounting(NUM_SLOTS, NUM_SLOTS);
	if (slots_sem == NULL) {
		xil_printf("Rcfg_Manager: Error while creating counting semaphore\n");
	}

	xil_printf("Rcfg_Manager: Setup complete. Ready to go!\n");

	return XST_SUCCESS;
}

Stopwatch rcfg_manager_execute_hw_op(const Hw_Op *hw_op)
{
	uint32_t slot_idx;
	UBaseType_t task_priority;

	Stopwatch watch;

	// Perform pre computation op (if required)
	slots_drv_pre_op(hw_op, logger_);

	// Save the calling task's priority
	task_priority = uxTaskPriorityGet(NULL);

	// Rise the calling task's priory to the maximum level
	vTaskPrioritySet(NULL, configMAX_PRIORITIES - 1);

	// Try to take a slot. Wait if there isn't one available
	slot_idx = take_slot_(hw_op);

	logger_log(	logger_, LOG_LEVEL_FULL,
			"Rcfg_Manager: Start hardware operation: "
			"\"%s\", reserved by task: \"%s\", in slot %u\n",
			hw_op->name, pcTaskGetTaskName(NULL), slot_idx);

	// Begin init/computation time
	stopwatch_start(&watch);

	// Accelerator reconfigured in the slot. Initialize, feed and start
	slots_drv_init_slot(slot_idx);

	// Start the computation. The calling task will be suspended until the computation is complete
	slots_drv_start_op(slot_idx, hw_op, logger_);

	// Stop computation time
	stopwatch_stop(&watch);

	// Free the slot
	free_slot(slot_idx, hw_op);

	logger_log(	logger_, LOG_LEVEL_SIMPLE,
			"Rcfg_Manager: Hardware operation: \"%s\", "
			"reserved by task: \"%s\", in slot %u completed in %.5f ms\n",
			hw_op->name, pcTaskGetTaskName(NULL), slot_idx, stopwatch_get_ms(&watch));


	// Restore the calling task's priority
	vTaskPrioritySet(NULL, task_priority);

	// Perform post computation op (if required)
	slots_drv_post_op(hw_op, logger_);

	return watch;
}
