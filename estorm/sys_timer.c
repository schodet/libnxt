#include "at91sam7s256.h"

#include "aic.h"

/* The board is clocked at 48MHz */
#define CLOCK_FREQ 48000000

/* The Periodic Interval Timer has a frequency of CLK/16. */
#define PIT_FREQ (CLOCK_FREQ/16)

void sys_timer_isr(void) {
}

void sys_timer_init(void) {
  /* Set the Periodic Interval Timer to a tiny interval, and set it
   * disabled. This way, it should wrap around and shut down quickly,
   * if it's already running.
   */
  *AT91C_PITC_PIMR = 1;

  /* Wait for the timer's internal counter to return to zero. */
  while (*AT91C_PITC_PIVR & AT91C_PITC_CPIV);

  /* Install the interrupt handler for the system timer, and tell the
   * AIC to handle that interrupt.
   */
  aic_install_isr(AT91C_ID_SYS, sys_timer_isr);
  aic_enable(AT91C_ID_SYS);

  /* Configure the Periodic Interval Timer with a frequency of
   * 1000Hz. The timer is enabled, and will raise the interrupt we
   * installed previously 1000 times a second.
   */
  *AT91C_PITC_PIMR = (((PIT_FREQ/1000)-1) |
                      AT91C_PITC_PITEN |
                      AT91C_PITC_PITIEN);
}