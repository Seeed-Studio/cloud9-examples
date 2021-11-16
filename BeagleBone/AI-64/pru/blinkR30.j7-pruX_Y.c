#include <stdint.h>
#include "resource_table_empty.h"
#include "prugpio.h"
#include <pru_cfg.h>
#include <pru_ctrl.h>


typedef struct {
	uint32_t ctrl;
	uint32_t duty;
	uint32_t cycle;
	uint32_t count;
}pwm_t;

volatile register unsigned int __R30;
volatile register unsigned int __R31;

#define MAXCH			7
#define DELAY_1US		25
#define CTRL_START		0x01
#define CTRL_POLARY_L	0x02

#define SHL(n)			((uint32_t)0x01<<(n))

#define PRU_SRAM __far __attribute__((cregister("PRU_SHAREDMEM", near)))
PRU_SRAM volatile pwm_t pwm[MAXCH] = { 0 };

int main(void)
{
	uint8_t i = 0;
	uint32_t pins[MAXCH] = { 0 };//{ SHL(5), SHL(13), SHL(16), SHL(17), SHL(2), SHL(8), SHL(4) };

	/* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
	/* C28 defaults to 0x00000000, we need to set bits 23:8 to 0x0100 in order to have
		it point to 0x00010000 */
	PRU0_CTRL.CTPPR0_bit.C28_BLK_POINTER = 0x0100;

	__R30 = 0x0;

	pins[0] = SHL(5);
	pins[1] = SHL(13);
	pins[2] = SHL(16);
	pins[3] = SHL(17);
	pins[4] = SHL(2);
	pins[5] = SHL(8);
	pins[6] = SHL(4);

	for (i=0; i<MAXCH; i++) {
		pwm[i].ctrl = CTRL_START;
		pwm[i].duty = 1;
		pwm[i].cycle = 2;
		pwm[i].count = 0x0;
	}

	for (;;) {
		for (i=0; i<MAXCH; i++) {
			if (!(pwm[i].ctrl & CTRL_START)) {
				if (pwm[i].ctrl & CTRL_POLARY_L)
					__R30 |= pins[i];
				else
					__R30 &= ~pins[i];
			
				continue;
			}

			if (pwm[i].count >= pwm[i].cycle) {
				pwm[i].count = 0;
			}

			if (pwm[i].count < pwm[i].duty) {
				if (pwm[i].ctrl & CTRL_POLARY_L)
					__R30 &= ~pins[i];
				else
					__R30 |= pins[i];
			}
			else {
				if (pwm[i].ctrl & CTRL_POLARY_L)
					__R30 |= pins[i];
				else
					__R30 &= ~pins[i];
			}
			pwm[i].count++;
		}
	}

	__halt();
}

// Sets pinmux
#pragma DATA_SECTION(init_pins, ".init_pins")
#pragma RETAIN(init_pins)
const char init_pins[] =  
	"\0\0";
