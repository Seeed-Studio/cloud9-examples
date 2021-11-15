#include <stdint.h>
#include "resource_table_empty.h"
#include "prugpio.h"
#include <pru_cfg.h>
#include <pru_ctrl.h>


volatile register unsigned int __R30;
volatile register unsigned int __R31;

//#define PRU_SRAM __far attribute((cregister("PRU_SHAREDMEM", near)))
#define PRU_SRAM __far __attribute__((cregister("PRU_SHAREDMEM", near)))
//#define PRU_DMEM0 __far __attribute__((cregister("PRU_DMEM_0_1", near)))
//#define PRU_DMEM1 __far __attribute__((cregister("PRU_DMEM_1_0", near)))

#if 1
//volatile uint32_t shared_0;
PRU_SRAM volatile uint32_t shared[8] = {0xaa, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
//PRU_DMEM0 volatile uint32_t shared_2;
//PRU_DMEM1 volatile uint32_t shared_3;
//#pragma DATA_SECTION(shared_4, ".bss")
//volatile uint32_t shared_4;

//#define PRU0_DRAM		0x00000 // offset to DRAM
//volatile unsigned int *shared_5 = (unsigned int *)(PRU0_DRAM + 0x200);

int main(void)
{
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
	PRU0_CTRL.CTPPR0_bit.C28_BLK_POINTER = 0x0100;

//	shared_1 = 0xdeadbeef;
	shared[0] = 0xdd;
	shared[1] = 0x01;
	shared[2] = 0x02;
	shared[3] = 0x03;
	shared[4] = 0x04;
	shared[5] = 0x05;
	shared[6] = 0x06;
	shared[7] = 0x07;

	__halt();
}
#else
// 500000000/5
void main(void) {
	//uint32_t gpio = P8_04;

	while(1) {
		__R30 = 0xFFFFFF;//0x1<<2;		// Set GPIO pins
		__delay_cycles(250);    // Wait 1/2 second
		__R30 = 0;			// Clear GPIO pins
		__delay_cycles(250);
		}
	__halt();
}

// Sets pinmux
#pragma DATA_SECTION(init_pins, ".init_pins")
#pragma RETAIN(init_pins)
const char init_pins[] =  
	"\0\0";
#endif
