//
// Zynq partial reconfiguration test code
// Marco Pagani - 2016 - marco.pag<#a#t#>outlook.com
//

#include "dev_cfg.h"
#include "xdevcfg.h"
#include "xil_printf.h"

#include "xscugic.h"
#include "xil_misc_psreset_api.h"

#include "FreeRTOS.h"
#include "task.h"

#define DEV_CFG_DEVICE_ID		XPAR_XDCFG_0_DEVICE_ID
#define	DEV_CFG_INTERRUPT_ID	XPAR_XDCFG_0_INTR

//-------------------------------------------------------------------------------

// SLCR registers
#define SLCR_LOCK	0xF8000004
#define SLCR_UNLOCK	0xF8000008
#define SLCR_LOCKSTA 0xF800000C
#define SLCR_LVL_SHFTR_EN 0xF8000900
#define SLCR_PCAP_CLK_CTRL XPAR_PS7_SLCR_0_S_AXI_BASEADDR + 0x168

#define SLCR_PCAP_CLK_CTRL_EN_MASK 0x1
#define SLCR_LOCK_VAL	0x767B
#define SLCR_UNLOCK_VAL	0xDF0D

#define DDR_URGENT 0xf8000600
#define DDR_URGENT_SEL 0xf800061c

// DDR controller configuration registers
#define AXI_PRIORITY_RD_PORT0 0xf8006218
#define AXI_PRIORITY_RD_PORT1 0xf800621c
#define AXI_PRIORITY_RD_PORT2 0xf8006220
#define AXI_PRIORITY_RD_PORT3 0xf8006224

#define AXI_PRIORITY_WR_PORT0 0xf8006208
#define AXI_PRIORITY_WR_PORT1 0xf800620c
#define AXI_PRIORITY_WR_PORT2 0xf8006210
#define AXI_PRIORITY_WR_PORT3 0xf8006214

//-------------------------------------------------------------------------------

#define REG_WRITE(BaseAddress, RegOffset, Data) \
    *(volatile uint32_t *)((BaseAddress) + (RegOffset)) = (uint32_t)(Data)

#define REG_READ(BaseAddress, RegOffset) \
    *(volatile uint32_t *)((BaseAddress) + (RegOffset))

//-------------------------------------------------------------------------------

#define FREE_RTOS

//-------------------------------------------------------------------------------

// Interrupt controller instance defined in the BSP file: portZynq7000.c
extern XScuGic xInterruptController;

// Configuration Device (DevC) instance
static XDcfg dev_cfg_;

// Only one task at a time can use the configuration port
static volatile TaskHandle_t xTaskToNotify = NULL;

static int interrupt_enabled_ = 0;

static
void dev_cfg_event_handler(void *CallBackRef, u32 IntrStatus)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (IntrStatus & XDCFG_IXR_D_P_DONE_MASK) {

#ifdef PRINT_DBG
		xil_printf("Dev_Cfg: Device programmed isr\n");
#endif

//		REG_WRITE(SLCR_UNLOCK, 0, SLCR_UNLOCK_VAL);
//		REG_WRITE(DDR_URGENT, 0, 0);
//		REG_WRITE(SLCR_LOCK, 0, SLCR_LOCK_VAL);

		// xTaskToNotify should not be NULL as a reconfiguration was in progress
		configASSERT( xTaskToNotify != NULL );

		// Signal the task that the computation is complete
		vTaskNotifyGiveFromISR( xTaskToNotify, &xHigherPriorityTaskWoken );

		// Clear
		xTaskToNotify = NULL;

		/* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
		should be performed to ensure the interrupt returns directly to the highest
		priority task.  The macro used for this purpose is dependent on the port in
		use and may be called portEND_SWITCHING_ISR(). */
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

	}
}

int dev_cfg_init()
{
	int retval;
	uint32_t status;
	XDcfg_Config *dev_cgf_config_;

//	REG_WRITE(AXI_PRIORITY_RD_PORT1, 0, 0x0f);
//	REG_WRITE(AXI_PRIORITY_WR_PORT1, 0, 0x0f);
//
//	REG_WRITE(AXI_PRIORITY_RD_PORT0, 0, 0x703FF);
//	REG_WRITE(AXI_PRIORITY_WR_PORT0, 0, 0x703FF);
//
//	REG_WRITE(AXI_PRIORITY_RD_PORT2, 0, 0x703FF);
//	REG_WRITE(AXI_PRIORITY_WR_PORT2, 0, 0x703FF);
//
//	REG_WRITE(AXI_PRIORITY_RD_PORT3, 0, 0x703FF);
//	REG_WRITE(AXI_PRIORITY_WR_PORT3, 0, 0x703FF);

	// Configure DevCfg
	dev_cgf_config_ = XDcfg_LookupConfig(DEV_CFG_DEVICE_ID);
	XDcfg_CfgInitialize(&dev_cfg_, dev_cgf_config_, dev_cgf_config_->BaseAddr);

	retval = XDcfg_SelfTest(&dev_cfg_);
	if (retval != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Enable the PCAP clock
	status = Xil_In32(SLCR_PCAP_CLK_CTRL);
	if (!(status & SLCR_PCAP_CLK_CTRL_EN_MASK)) {
		REG_WRITE(SLCR_UNLOCK, 0, SLCR_UNLOCK_VAL);
		REG_WRITE(SLCR_PCAP_CLK_CTRL, 0, (retval | SLCR_PCAP_CLK_CTRL_EN_MASK));
		REG_WRITE(SLCR_LOCK, 0, SLCR_LOCK_VAL);
	}

	// Enable and select PCAP interface for partial reconfiguration
	XDcfg_EnablePCAP(&dev_cfg_);
	XDcfg_SetControlRegister(&dev_cfg_, XDCFG_CTRL_PCAP_PR_MASK);

	return XST_SUCCESS;
}

// The interrupt controller XScuGic should be already configured
int dev_cfg_setup_interrupt()
{
	int result;

	interrupt_enabled_ = 1;

	// Connect the DevC ISR to the exception table
	result = XScuGic_Connect(&xInterruptController, DEV_CFG_INTERRUPT_ID,
							(Xil_InterruptHandler)XDcfg_InterruptHandler, &dev_cfg_);

	if (result != XST_SUCCESS) {
		return result;
	}

	// Set the driver event handler
	XDcfg_SetHandler(&dev_cfg_, (void *)dev_cfg_event_handler, &dev_cfg_);

	//print("Enable the ISR\n\r");
	XScuGic_Enable(&xInterruptController, DEV_CFG_INTERRUPT_ID);

	return XST_SUCCESS;
}

int dev_cfg_transfer_bitfile(const void *bitfile_addr, uint32_t bitfile_len)
{
	uint32_t status;

#ifdef FREE_RTOS
	// For the FreeRTOS notification mechanism (used as a binary semaphore)
	uint32_t ulNotificationValue;
#endif

	// Clear DMA and PCAP Done Interrupts
	XDcfg_IntrClear(&dev_cfg_, (XDCFG_IXR_PCFG_DONE_MASK | XDCFG_IXR_D_P_DONE_MASK | XDCFG_IXR_DMA_DONE_MASK));

	// Check if DMA command queue is full
	status = XDcfg_ReadReg(dev_cfg_.Config.BaseAddr, XDCFG_STATUS_OFFSET);
	if ((status & XDCFG_STATUS_DMA_CMD_Q_F_MASK) == XDCFG_STATUS_DMA_CMD_Q_F_MASK) {
		return XST_FAILURE;
	}

	// Enable the Device Programming done interrupt
	XDcfg_IntrEnable(&dev_cfg_, XDCFG_IXR_D_P_DONE_MASK);

	// xTaskToNotify should be NULL as no computation is in progress
	configASSERT( xTaskToNotify == NULL );

	// Store the handle of the calling task
	xTaskToNotify = xTaskGetCurrentTaskHandle();

//	REG_WRITE(SLCR_UNLOCK, 0, SLCR_UNLOCK_VAL);
//	REG_WRITE(DDR_URGENT, 0, (1u << 5) || (1u << 1));
//	REG_WRITE(SLCR_LOCK, 0, SLCR_LOCK_VAL);

	// Start bitstream transfer from DDR into fabric in non secure mode
	XDcfg_Transfer(	&dev_cfg_, (uint32_t *)bitfile_addr, (bitfile_len >> 2),
					(uint32_t *)XDCFG_DMA_INVALID_ADDRESS, 0, XDCFG_NON_SECURE_PCAP_WRITE);


	if (!interrupt_enabled_) {
		while ((XDcfg_IntrGetStatus(&dev_cfg_) & XDCFG_IXR_D_P_DONE_MASK) != XDCFG_IXR_D_P_DONE_MASK) {
			// Wait
		}

	} else {

#ifdef FREE_RTOS
		// Wait to be notified that the reconfiguration is complete.
		// (pdTRUE, which has the effect of clearing the task's notification
		// value back to 0, making the notification value act like a binary semaphore)
		ulNotificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		//*** Now the Hw accelerator is configured in the slot and ready to be used ***//
		if (ulNotificationValue != 1) {
			xil_printf("Rcfg_Manager: Reconfiguration notification error\n");
		}
#endif

	}

	return XST_SUCCESS;
}



