/* 
 * sysclock.c - configuration routines for KL25Z system clock
 * 
 * Author Howdy Pierce, howdy.pierce@colorado.edu
 *
 * See section 24 of the KL25Z Reference Manual to understand this code
 *
 * Inspired by https://learningmicro.wordpress.com/configuring-device-clock-and-using-systick-system-tick-timer-module-to-generate-software-timings/

 */

#include "MKL25Z4.h"
#include "sysclock.h"


void
sysclock_init()
{
  // Corresponds to FEI (FLL Engaged Internal) mode as shown in sec 24.4.1

  // Select PLL/FLL as clock source 
  MCG->C1 &= ~(MCG_C1_CLKS_MASK);
  MCG->C1 |= MCG_C1_CLKS(0);  // "Encoding 0 — Output of FLL or PLL is selected (depends on PLLS control bit)."
	
  // Use internal reference clock as source for the FLL
  MCG->C1 |= MCG_C1_IREFS(1);  // "1  The slow internal reference clock is selected."

  // Select the FLL (by setting "PLL select" to 0)
  MCG->C6 &= ~(MCG_C6_PLLS_MASK);
  MCG->C6 |= MCG_C6_PLLS(0);  // "0   FLL is selected"

  // Select 24 MHz - see table for MCG_C4[DMX32]
  MCG->C4 &= ~(MCG_C4_DRST_DRS_MASK & MCG_C4_DMX32_MASK);
  MCG->C4 |= MCG_C4_DRST_DRS(0);
  MCG->C4 |= MCG_C4_DMX32(1);
}

