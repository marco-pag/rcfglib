
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "xil_cache.h"		// Cache
#include "xparameters.h"	// PS peripherals parameters
#include "xscugic.h" 		// PS Interrupt controller

#include "xslot_0.h"		// Slot 0 driver
#include "xslot_1.h"		// Slot 1 driver

#include "dev_cfg.h"
#include "display.h"
#include "image.h"

#include "hw_op.h"
#include "stopwatch.h"
#include "parameters.h"

#include "slots_drv.h"

#ifdef FREE_RTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

//#define PRINT_LOG
#define PRINT_LOG_OPS

#define write_reg(BaseAddress, RegOffset, Data) \
    *(volatile unsigned int *)((BaseAddress) + (RegOffset)) = (unsigned int)(Data)

#define read_reg(BaseAddress, RegOffset) \
    *(volatile unsigned int *)((BaseAddress) + (RegOffset))


// Slot 0 and decoupler reg parameters
#define	SLOT_0_DEVICE_ID		XPAR_SLOT_0_0_DEVICE_ID
#define	SLOT_0_INTERRUPT_INTR	XPAR_FABRIC_SLOT_0_0_INTERRUPT_INTR
#define SLOT_0_BUS_BASE_ADDR	XPAR_SLOT_0_0_S_AXI_CTRL_BUS_BASEADDR
#define SLOT_0_ARGS				XSLOT_0_CTRL_BUS_ADDR_ARGS_BASE
#define SLOT_0_AP_CTRL			XSLOT_0_CTRL_BUS_ADDR_AP_CTRL
#define SLOT_0_ISR				XSLOT_0_CTRL_BUS_ADDR_ISR
#define SLOT_0_IER				XSLOT_0_CTRL_BUS_ADDR_IER
#define SLOT_0_GIE				XSLOT_0_CTRL_BUS_ADDR_GIE
#define SLOT_0_ID				XSLOT_0_CTRL_BUS_ADDR_ID_DATA
#define SLOT_0_DECOUPLER		XPAR_PR_DECOUPLER_SLOT_0_BASEADDR

// Slot 1 and decoupler reg parameters
#define	SLOT_1_DEVICE_ID		XPAR_SLOT_1_0_DEVICE_ID
#define	SLOT_1_INTERRUPT_INTR	XPAR_FABRIC_SLOT_1_0_INTERRUPT_INTR
#define SLOT_1_BUS_BASE_ADDR	XPAR_SLOT_1_0_S_AXI_CTRL_BUS_BASEADDR
#define SLOT_1_ARGS				XSLOT_1_CTRL_BUS_ADDR_ARGS_BASE
#define SLOT_1_AP_CTRL			XSLOT_1_CTRL_BUS_ADDR_AP_CTRL
#define SLOT_1_ISR				XSLOT_1_CTRL_BUS_ADDR_ISR
#define SLOT_1_IER				XSLOT_1_CTRL_BUS_ADDR_IER
#define SLOT_1_GIE				XSLOT_1_CTRL_BUS_ADDR_GIE
#define SLOT_1_ID				XSLOT_1_CTRL_BUS_ADDR_ID_DATA
#define SLOT_1_DECOUPLER		XPAR_PR_DECOUPLER_SLOT_1_BASEADDR

typedef struct Slot_Addresses_ {
	uint32_t self_idx;
	uint32_t device_id;
	uint32_t interrupt_intr;
	uint32_t axi_base_addr;
	uint32_t ap_crtl;
	uint32_t args;
	uint32_t isr;
	uint32_t ier;
	uint32_t gie;
	uint32_t id;
	uint32_t decoupler;
} Slot_Addresses;

static Slot_Addresses slot0_addrs = {
	.self_idx = 0,
	.device_id = SLOT_0_DEVICE_ID,
	.interrupt_intr = SLOT_0_INTERRUPT_INTR,
	.axi_base_addr = SLOT_0_BUS_BASE_ADDR,
	.ap_crtl = SLOT_0_AP_CTRL,
	.args = SLOT_0_ARGS,
	.isr = SLOT_0_ISR,
	.ier = SLOT_0_IER,
	.gie = SLOT_0_GIE,
	.id = SLOT_0_ID,
	.decoupler = SLOT_0_DECOUPLER
};

static Slot_Addresses slot1_addrs = {
	.self_idx = 1,
	.device_id = SLOT_1_DEVICE_ID,
	.interrupt_intr = SLOT_1_INTERRUPT_INTR,
	.axi_base_addr = SLOT_1_BUS_BASE_ADDR,
	.ap_crtl = SLOT_1_AP_CTRL,
	.args = SLOT_1_ARGS,
	.isr = SLOT_1_ISR,
	.ier = SLOT_1_IER,
	.gie = SLOT_1_GIE,
	.id = SLOT_1_ID,
	.decoupler = SLOT_1_DECOUPLER
};

static Slot_Addresses *slots_addrs[NUM_SLOTS] = {&slot0_addrs, &slot1_addrs};

// ---------------- Private module variables ---------------- //

// Test
volatile static int hw_mod_return_avail_[NUM_SLOTS] = { 0 };

// Test
volatile static int interrupt_enabled_[NUM_SLOTS] = { 0 };

// Store the task handle of the tasks that are current waiting for
// the hardware computation. One task handle for each slot
#ifdef FREE_RTOS
static TaskHandle_t tasks_to_notify_[NUM_SLOTS] = { 0 };
#endif

#ifdef FREE_RTOS
// Interrupt controller instance defined in the BSP file: portZynq7000.c
extern XScuGic xInterruptController;
#else
XScuGic xInterruptController;
#endif

// ---------------- Private functions ---------------- //
static inline
void slots_drv_run_(uint32_t slot_idx)
{
	uint32_t reg;

	reg = read_reg(slots_addrs[slot_idx]->axi_base_addr, slots_addrs[slot_idx]->ap_crtl) & 0x80;
	write_reg(slots_addrs[slot_idx]->axi_base_addr, slots_addrs[slot_idx]->ap_crtl, reg | 0x01);
}

// Must be reentrant!
void slots_drv_isr_(uint32_t *self_idx)
{
#ifdef FREE_RTOS
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif

	uint32_t slot_idx = *self_idx;

	// Clear the local interrupt
	write_reg(slots_addrs[slot_idx]->axi_base_addr, slots_addrs[slot_idx]->isr, 0x1);

	// Result available
	hw_mod_return_avail_[slot_idx] = 1;

	//xil_printf("Slots_Drv: Hw_mod isr return\n");

#ifdef FREE_RTOS
	// At this point tasks_to_notify should not be NULL as a transmission was in progress
	configASSERT(tasks_to_notify_[slot_idx] != NULL);

	// Notify the task that the computation is complete
	vTaskNotifyGiveFromISR(tasks_to_notify_[slot_idx], &xHigherPriorityTaskWoken);

	// Clear task name
	tasks_to_notify_[slot_idx] = NULL;

	/* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
	should be performed to ensure the interrupt returns directly to the highest
	priority task.  The macro used for this purpose is dependent on the port in
	use and may be called portEND_SWITCHING_ISR(). */
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif
}

static inline
void slots_drv_start_compute_(uint32_t slot_idx, const uint32_t args[], Logger *logger)
{
	int i;
	uint32_t reg;

#ifdef PRINT_LOG
	if (logger) {
		logger_enqueue_msg(logger, "Slots_Drv: Hw_Mod start computing \n");
	}
#endif

	// Feed arguments to the accelerator trough AXI control bus
	for (i = 0; i < HW_OP_ARGS_SIZE; i++) {
		write_reg(	slots_addrs[slot_idx]->axi_base_addr,
					slots_addrs[slot_idx]->args + i * sizeof(args_t),
					args[i]);
	}

	// Check if the module is ready
	reg = read_reg(slots_addrs[slot_idx]->axi_base_addr, slots_addrs[slot_idx]->ap_crtl);
	if (reg & 0x1) {
		print("Slots_Drv: Error: Module is not ready! \n\r");
		exit(-1);
	}

#ifdef FREE_RTOS
	// tasks_to_notify should be NULL as no computation is in progress
	// in the hardware module inside the slot
	configASSERT(tasks_to_notify_[slot_idx] == NULL);

	// Store the handle of the calling task
	tasks_to_notify_[slot_idx] = xTaskGetCurrentTaskHandle();
#endif

	// Start the computation
	slots_drv_run_(slot_idx);

	// Polling mode
	if (!interrupt_enabled_[slot_idx]) {

#ifdef PRINT_LOG
		if (logger) {
			logger_enqueue_msg(logger, "Slots_Drv: Polling mode\n");
		}
#endif

		do {
			reg = read_reg(slots_addrs[slot_idx]->axi_base_addr, slots_addrs[slot_idx]->ap_crtl);
			reg = (reg >> 1) & 0x1;
		} while (!reg);

#ifdef PRINT_LOG
		if (logger) {
			logger_enqueue_msg(logger, "Slots_Drv: Result received.\n\r");
		}
#endif

	}
}

// ---------------- Public functions ---------------- //
int slots_drv_init_slot(uint32_t slot_idx)
{
	uint32_t reg;

	if (interrupt_enabled_[slot_idx]) {

		// Enable interrupt
		reg =  read_reg(slots_addrs[slot_idx]->axi_base_addr, slots_addrs[slot_idx]->ier);
		write_reg(slots_addrs[slot_idx]->axi_base_addr, slots_addrs[slot_idx]->ier, reg | 0x1);


		// Enable global interrupt
		write_reg(slots_addrs[slot_idx]->axi_base_addr, slots_addrs[slot_idx]->gie, 1);
	}

	return XST_SUCCESS;
}

uint32_t slots_drv_get_id(uint32_t slot_idx)
{
	return read_reg(slots_addrs[slot_idx]->axi_base_addr, slots_addrs[slot_idx]->id);
}

void slots_drv_start_op(uint32_t slot_idx, const Hw_Op *hw_op, Logger *logger)
{
#ifdef FREE_RTOS
	// For the FreeRTOS notification mechanism (used as a binary semaphore)
	uint32_t ulNotificationValue;
#endif

	slots_drv_start_compute_(slot_idx, hw_op->args, logger);

#ifdef FREE_RTOS
	// Suspend and wait to be notified that the computation is complete
	ulNotificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	// Computation is complete
	if (ulNotificationValue != 1) {
		xil_printf("Slot_Drv: Computation notification error\n");
		exit(-1);
	}
#endif
}


void slots_drv_pre_op(const Hw_Op *hw_op, Logger *logger)
{
#ifdef PRINT_LOG_OPS
	char p_string[256];
	Stopwatch watch;
#endif

	if (hw_op->pre_op) {

#ifdef PRINT_LOG_OPS

		sprintf(p_string, "Slot_Drv: Hardware operation: \"%s\","
				"reserved by task: \"%s\", pre_op started\n",
				hw_op->name, pcTaskGetTaskName(NULL));

		if (logger)
			logger_enqueue_msg(logger, p_string);

		stopwatch_start(&watch);
#endif
		// Hw operation pre-operation
		hw_op->pre_op(hw_op);

#ifdef PRINT_LOG_OPS
		stopwatch_stop(&watch);

		sprintf(p_string, "Slot_Drv: Hardware operation: \"%s\","
				"reserved by task: \"%s\", pre_op complete, time %.5f ms\n",
				hw_op->name, pcTaskGetTaskName(NULL), stopwatch_get_ms(&watch));

		if (logger)
			logger_enqueue_msg(logger, p_string);
#endif
	}
}

void slots_drv_post_op(const Hw_Op *hw_op, Logger *logger)
{
#ifdef PRINT_LOG_OPS
	char p_string[256];
	Stopwatch watch;
#endif

	if (hw_op->post_op) {

#ifdef PRINT_LOG_OPS

		sprintf(p_string, "Slot_Drv: Hardware operation: \"%s\","
				"reserved by task: \"%s\", post_op started\n",
				hw_op->name, pcTaskGetTaskName(NULL));

		if (logger)
			logger_enqueue_msg(logger, p_string);

		stopwatch_start(&watch);
#endif

		// Hw operation post-operation
		hw_op->post_op(hw_op);

#ifdef PRINT_LOG_OPS
		stopwatch_stop(&watch);

		sprintf(p_string, "Slot_Drv: Hardware operation: \"%s\","
				"reserved by task: \"%s\", post_op complete, time %.5f ms\n",
				hw_op->name, pcTaskGetTaskName(NULL), stopwatch_get_ms(&watch));

		if (logger)
			logger_enqueue_msg(logger, p_string);
#endif
	}
}


void slots_drv_decouple_slot(uint32_t slot_idx)
{
	// Detach interrupt at interrupt controller level
	XScuGic_Disable(&xInterruptController, slots_addrs[slot_idx]->interrupt_intr);

	// Enable partition decoupler (decouple reconfigurable partition)
	write_reg(slots_addrs[slot_idx]->decoupler, 0, 1);

	if (read_reg(slots_addrs[slot_idx]->decoupler, 0) != 1) {
		xil_printf("Slots_Drv: Decoupler error\n");
		exit(-1);
	}

}

void slots_drv_couple_slot(uint32_t slot_idx)
{
	// Re attach interrupt at interrupt controller level
	XScuGic_Enable(&xInterruptController, slots_addrs[slot_idx]->interrupt_intr);

	// Disable partition decoupler
	write_reg(slots_addrs[slot_idx]->decoupler, 0, 0);

	if (read_reg(slots_addrs[slot_idx]->decoupler, 0) != 0) {
		xil_printf("Slots_Drv: Decoupler error\n");
		exit(-1);
	}
}

void init_exception_sys()
{
	// Initialize the exception handlers
	Xil_ExceptionInit();

	// Connect the interrupt controller interrupt handler to the hardware interrupt handling logic in the processor
	Xil_ExceptionRegisterHandler(	XIL_EXCEPTION_ID_IRQ_INT,
									(Xil_ExceptionHandler)XScuGic_InterruptHandler,
									&xInterruptController);

	//Enable the exception handler
	Xil_ExceptionEnable();
}

int slots_drv_setup_interrupts()
{
	int slot_idx, result;

	for (slot_idx = 0; slot_idx < NUM_SLOTS; ++slot_idx) {
		interrupt_enabled_[slot_idx] = 1;
	}

	// The interrupt controller XScuGic is already configured by freertos
#ifndef FREE_RTOS
	XScuGic_Config *scu_cfg = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (scu_cfg == NULL) {
		print("Slots_Drv: Interrupt Configuration Lookup Failed\n\r");
		return XST_FAILURE;
	}

	result = XScuGic_CfgInitialize(&xInterruptController, scu_cfg, scu_cfg->CpuBaseAddress);
	if (result != XST_SUCCESS) {
		return result;
	}

	// Self-test
	result = XScuGic_SelfTest(&xInterruptController);
	if (result != XST_SUCCESS) {
		return result;
	}
#endif

	for (slot_idx = 0; slot_idx < NUM_SLOTS; ++slot_idx) {
		// Connect the hardware accelerator module ISR to the exception table
		//print("Connect the hardware accelerator ISR to the Exception handler table\n\r");
		result = XScuGic_Connect(	&xInterruptController,
									slots_addrs[slot_idx]->interrupt_intr,
									(Xil_InterruptHandler)slots_drv_isr_,
									&(slots_addrs[slot_idx]->self_idx));

		if (result != XST_SUCCESS) {
			return result;
		}
		//print("Enable the ISR\n\r");
		XScuGic_Enable(&xInterruptController, slots_addrs[slot_idx]->interrupt_intr);
	}

	return XST_SUCCESS;
}
