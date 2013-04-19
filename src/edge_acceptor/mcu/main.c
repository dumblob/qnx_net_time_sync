/**
 * counter of rising edges on certain ports
 * Jan Pacner
 * 2012-12-12 18:00:17 CET
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xpacne00@stud.fit.vutbr.cz> wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 */

// NOTE
//   interrupt latency is 6 cycles for MSP430x (== MSP430 with more than
//   64k address space)

// RS232 voltage levels etc.
// http://www.epanorama.net/circuits/rspower.html
// http://en.wikibooks.org/wiki/Serial_Programming/8250_UART_Programming

#if FITKIT_VERSION == 2

// header order matters
#include <msp430x261x.h>
#include <fitkitlib.h>

//#include <io.h>//FIXME

#include <stdio.h>
#include <limits.h>

// mspgcc does not have the following symbols defined :(
#ifndef P1SEL2_
#define P1SEL2_ 0x0041
sfrb(P1SEL2, P1SEL2_);
#endif
#ifndef P2SEL2_
#define P2SEL2_ 0x0042
sfrb(P2SEL2, P2SEL2_);
#endif

// msp430 on FitKit has 8kB RAM
#define MEASUREMENTS 500  // 1 measurement == 2x 4B == 8B
                          // => maximum of cca 700 measurements
#define COUNTER_MAX 0xFFFF

#define MASTER BIT3
#define SLAVE  BIT3

static uint16_t COUNTER32_LOWER = 0;
#define COUNTER16 TAR
#define COUNTER32 \
  ((uint32_t)(((uint32_t)COUNTER32_LOWER) << 16) | (uint32_t)COUNTER16)

static uint32_t master[MEASUREMENTS] = { 0 };
static uint32_t slave[MEASUREMENTS]  = { 0 };

static enum {
  RUNNING,
  FINISHED,
} state = RUNNING;

// see msp430x26x.h (yes, NOT msp430x261x.h, which is FitKit-specific),
//   find *_VECTOR constants and cut off _VECTOR postfix (yes, this
//   is another FitKit specific)
ISR(TIMERA1, isr_timera1) {
  TACTL &= ~BIT0;  // TAIFG; set no interrupt pending
  ++COUNTER32_LOWER;
  /* FIXME
  switch( TAIV ) {
    //case 2:  // CCR1
    //case 4:  // CCR2
    case 10:  // overflow
      break;
  }
  */
}

// master
ISR(PORT1, isr_port1) {
  P1IFG &= ~MASTER;  // clear the interrupt flag for a specific pin

  static uint16_t edges = 0;

  if (edges < MEASUREMENTS)
    master[edges++] = COUNTER32;
  else
    state = FINISHED;
}

// slave
ISR(PORT2, isr_port2) {
  P2IFG &= ~SLAVE;

  static uint16_t edges = 0;

  if (edges < MEASUREMENTS)
    slave[edges++] = COUNTER32;
  else
    state = FINISHED;
}

void print_user_help(void) {
  term_send_str_crlf("SYNOPSIS");
  term_send_str_crlf("  Measure N-times clock tics count from program launch");
  term_send_str_crlf("  to currently catched RS232 DTR rising edge.");
}

unsigned char decode_user_cmd(char *cmd_ucase, char *cmd) {
  return 0;
}

void fpga_initialized(void) {
}

int main(void) {
  initialize_hardware();

  #define FREQ 7372800 // Hz; must be corelated with number of
                       // measurements, becase we have only 32b counter
  //BCSCTL2 = SELM_2 | SELS | DIVM_0 | DIVS_1;  // MCLK = XT2CLK = 14.745 MHz, SMCLK = XT2CLK/2 = 7.3728 MHz
  //BCSCTL2 = SELM_2 | SELS | DIVM_1 | DIVS_1;  // MCLK = XT2CLK/2 = 7.3728 MHz, SMCLK = XT2CLK/2 = 7.3728 MHz

  WDG_stop();
  //WDTCTL = WDTPW + WDTHOLD;

  TACTL = 0;
  //TACTL &= ~(BIT5 | BIT4);  // MCx; stop timer (needed for reliable configuration)
  TACTL |= BIT2;            // TACLR; clear TAR, clock divider and count direction
  // increment period 1/SMCLK seconds [1/14.745 MHz = 0.5425us]
  TACTL |= BIT9;            // TASSELx; set clock source [SMCLK]
  //TACTL &= ~(BIT7 | BIT6);  // IDx; set no divider
  TACTL |= BIT1;            // TAIE; INT enable

  if (COUNTER_MAX == 0xFFFF) {
    TACTL |= BIT5;  // MCx; start repeatedly counting from 0 to 0xFFFF
  }
  else {
    TACCR0 = COUNTER_MAX;  // restart timer; set max value
    TACCTL0 = BIT4;  // CCIE; CCR0 INT enable
    TACTL |= BIT4;  // MCx; start repeatedly counting from 0 to TACCR0
  }

  __enable_interrupt();  // enable all interrupts (GIE -> 1)

  print_user_help();

  // master P1M3 (1.3; JP9 30)
  P1DIR &= ~MASTER;  // input pin
  //P1OUT |= MASTER;   // FIXME set pull up resistor on pin
  //P1REN |= MASTER;   // FIXME enable pullup/pulldown resistor on pin
  P1OUT &= ~MASTER;  // unset pull up resistor on pin
  P1REN &= ~MASTER;  // disable pullup/pulldown resistor on pin
  P1SEL = 0;         // I/O function on all pins (if any pin is 1, interrupts are disabled)
  P1SEL2 = 0;        // I/O function on all pins (if any pin is 1, interrupts are disabled)
  P1IES &= ~MASTER;   // low2high transition (negative voltage doesn't count)
  //P1IES |= MASTER;   // high2low transition [pin itself 0V]
  P1IFG &= ~MASTER;  // clear the interrupt flag for a specific pin
  P1IE |= MASTER;    // enable interrupts for a specific pin

  // slave P2M3 (2.3; JP9 22)
  P2DIR &= ~SLAVE;  // input pin
  //P1OUT |= SLAVE;   // FIXME set pull up resistor on pin
  //P2REN |= SLAVE;   // FIXME enable pullup/pulldown resistor on pin
  P1OUT &= ~SLAVE;  // unset pull up resistor on pin
  P2REN &= ~SLAVE;  // disable pullup/pulldown resistor on pin
  P2SEL = 0;        // I/O function on all pins (if any pin is 1, interrupts are disabled)
  P2SEL2 = 0;       // I/O function on all pins (if any pin is 1, interrupts are disabled)
  P2IES &= ~SLAVE;   // low2high transition
  //P2IES |= SLAVE;   // high2low transition [pin itself 0V]
  P2IFG &= ~SLAVE;  // clear the interrupt flag for a specific pin
  P2IE |= SLAVE;    // enable interrupts for a specific pin

  set_led_d5(1);

  /*
  pull-up    => 3.3V
  no pull-up => 0.3V

  -DTR => -8.542
  +DTR => 8.98
  */

  // wait until arrays are filled (one of them will have (MAX -1) items in
  //   most cases)
  while (state != FINISHED);

  P1IE &= ~MASTER;   // disable interrupts for a specific pin
  P2IE &= ~SLAVE;    // disable interrupts for a specific pin
  TACTL &= ~(BIT5 | BIT4);  // MCx; stop timer

  set_led_d6(1);

  static char s[64];
  snprintf(s, 64, "current freq: %lu Hz\n", (uint32_t)FREQ);
  term_send_str_crlf(s);

  uint16_t i;
  for (i = 0; i < MEASUREMENTS; ++i) {
    sprintf(s, "M: %lu S: %lu", master[i], slave[i]);
    term_send_str_crlf(s);
  }
  term_send_str_crlf("\nEND");

  //_BIS_SR(LPM0_bits + GIE);  // Enter LPM0 w/ interrupt
  for (;;)
    terminal_idle();
}

#else

int main(void) {
  return 0;
}
void fpga_initialized(void) {
}
void print_user_help(void) {
}
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd) {
  return 0;
}

#endif
