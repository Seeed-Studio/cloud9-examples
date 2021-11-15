#include <stdint.h>
#include "resource_table_empty.h"
#include "prugpio.h"
#include <pru_cfg.h>
#include <pru_ctrl.h>


volatile register unsigned int __R30;
volatile register unsigned int __R31;

#define MAXCH				1
#define DELAY_1us			25
#define CTRL_START			0x01
#define CTRL_POLARY_L		0x02

#define SHL(n)				((uint32_t)0x01<<(n))

volatile uint32_t *pwm = (uint32_t *)(0x10000);
#define pwm_ctl(j)			pwm[j+0]
#define pwm_dut(j)			pwm[j+1]
#define pwm_cyc(j)			pwm[j+2]
#define pwm_cnt(j)			pwm[j+3]


int main(void)
{
	uint8_t i = 0, j = 0;
	uint32_t pins[MAXCH] = { SHL(5), SHL(13), SHL(16), SHL(17), SHL(2), SHL(8), SHL(4) };

	/* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
	/* C28 defaults to 0x00000000, we need to set bits 23:8 to 0x0100 in order to have
		it point to 0x00010000 */
	PRU0_CTRL.CTPPR0_bit.C28_BLK_POINTER = 0x0100;

	__R30 = 0x0;

#pragma UNROLL(MAXCH)
	for (i=0; i<MAXCH; i++) {
		j = i*4;
		pwm_ctl(j) = CTRL_START; // ctrl
		pwm_dut(j) = 1; // duty
		pwm_cyc(j) = 2; // cycle
		pwm_cnt(j) = 0; // count
	}

	for (;;) {
		for (i=0; i<MAXCH; i++) {
			j = i*4;
			if (!(pwm_ctl(j) & CTRL_START)) {
				if (pwm_ctl(j) & CTRL_POLARY_L) {
					__R30 |= pins[i];
				}
				else {
					__R30 &= ~pins[i];
				}
				continue;
			}

			if (pwm_cnt(j) >= pwm_cyc(j)) {
				pwm_cnt(j) = 0;
			}

			if (pwm_cnt(j) < pwm_dut(j)) {
				if (pwm_ctl(j) & CTRL_POLARY_L) {
					__R30 &= ~pins[i];
				}
				else {
					__R30 |= pins[i];
				}
			}
			else {
				if (pwm_ctl(j) & CTRL_POLARY_L) {
					__R30 |= pins[i];
				}
				else {
					__R30 &= ~pins[i];
				}
			}
			pwm_cnt(j)++;
		}
	}

	__halt();
}

// Sets pinmux
#pragma DATA_SECTION(init_pins, ".init_pins")
#pragma RETAIN(init_pins)
const char init_pins[] =  
	"\0\0";
