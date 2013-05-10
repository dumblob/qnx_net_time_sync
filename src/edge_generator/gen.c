/**
 * periodical generator of rising edges on RS232 DTR pin
 * Jan Pacner
 * 2013-01-06 16:48:43 CET
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xpacne00@stud.fit.vutbr.cz> wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 */

// NOTE
//   disable System Management Mode (SMM) in BIOS else unpredictable 100us
//     interrupt latencies occur
//   a typical example is USB legacy support

// RAW serial programming (without 8250 driver) on QNX
// http://www.openqnx.com/phpbbforum/viewtopic.php?f=11&t=8953&sid=0c13b5a98982a7bbff37a3ccb0026561&start=15

// RS232 simple & short but full-blown explained
// http://www.commfront.com/RS232_Protocol_Analyzer_Monitor/RS232_Analyzer_Monitor_Tester_TUTORIAL.HTM

// RS232 voltage levels and wiring
// http://www.arcelect.com/rs232.htm

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>  // open()
#include <fcntl.h>  // open()
#include <sys/mman.h>  // mlock()

#include <devctl.h>
#include <sys/dcmd_chr.h>
#include <hw/inout.h>  // in8()
#include <sys/neutrino.h>  // ClockPeriod()
#include <stdint.h>  // uint8_t uint64_t
#include <time.h>  // clock_getres()
//#include <atomic.h>  // atomic_add() atomic_sub()

#define BIT0 0x1
#define BIT1 0x2
#define BIT2 0x4
#define BIT3 0x8
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

// interrupt vectors
#define SYS_CLOCK 0  // on x86: clock with resolution set by ClockPeriod()
#define COM1 4

// http://en.wikipedia.org/wiki/COM_(hardware_interface)
#define COM1_BASE_REG 0x3f8

// serial 8250 registers offsets (http://www.lammertbies.nl/comm/info/serial-uart.html)
#define SER_8250_REG_RX 0  // receive register
//#define SER_8250_REG_IE 1  // interrupt enable register
#define SER_8250_REG_II 2  // interrupt information register
//#define SER_8250_REG_LC 3  // line control register
#define SER_8250_REG_MC 4  // modem control register
//#define SER_8250_REG_LS 5  // line status register
//#define SER_8250_REG_MS 6  // modem status register
//#define SER_8250_REG_SC 7  // scratch register

#define SER_8250_IIR_NO_INT_PENDING BIT0
#define SER_8250_IIR_RX_AVAILABLE   BIT2

// 1000000ns by default; minimal supported value by QNX is 10000ns
#define CLOCK_PERIOD_NS 40000
#define DTR_PERIOD_NS   1000000000
//#define DTR_PERIOD_NS 130000000000

void check_statuses(int fd) {
  int data = 0;

  if (devctl(fd, DCMD_CHR_LINESTATUS, &data, sizeof(data), NULL)) {
    fprintf(stderr, "Error getting statuses.\n");
    perror(NULL);
    exit(EXIT_FAILURE);
  }

  if (data & _LINESTATUS_SER_RTS)
    printf("+RTS ");
  else
    printf("-RTS ");

  if (data & _LINESTATUS_SER_DTR)
    printf("+DTR ");
  else
    printf("-DTR ");

  if (data & _LINESTATUS_SER_CTS)
    printf("+cts ");
  else
    printf("-cts ");

  if (data & _LINESTATUS_SER_DSR)
    printf("+dsr ");
  else
    printf("-dsr ");

  if (data & _LINESTATUS_SER_RI)
    printf("+ri ");
  else
    printf("-ri ");

  if (data & _LINESTATUS_SER_CD)
    printf("+cd ");
  else
    printf("-cd ");

  printf("\n");
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("USAGE: %s <serX>\n", argv[0]);
    return EXIT_SUCCESS;
  }

  // ensure the same ClockPeriod on each of measurement stations
  struct _clockperiod new_clockperiod = {
    .nsec = CLOCK_PERIOD_NS,  // nanoseconds
    .fract = 0,  // reserved
  };

  if (ClockPeriod(CLOCK_REALTIME, &new_clockperiod, NULL, 0) == -1) {
    fprintf(stderr, "ERR ClockPeriod(CLOCK_REALTIME....\n");
    perror(NULL);
    return EXIT_FAILURE;
  }

  struct timespec ts;  // nsec2timespec()

  // only to inform user of the accurate period (which is a little bit
  //   different from the one set)
  if (clock_getres(CLOCK_REALTIME, &ts) == -1) {
    fprintf(stderr, "ERR clock_getres(CLOCK_REALTIME....\n");
    perror(NULL);
    return EXIT_FAILURE;
  }

  printf( "ClockPeriod(): %ld\n", ts.tv_nsec);

  #define BUF_SIZE 512
  char buf[BUF_SIZE];
  // -ihflow and -ohflow turn off flow control provided by hardware handshaking lines
  // TODO use struct termios instead
  snprintf(buf, BUF_SIZE, "stty baud=115200 bits=8 par=none stopb=1"
     " -ihflow -ohflow -isflow -osflow -clocal"
     " -ihpaged -ohpaged -ispaged -ospaged +flush"
     " < %s", argv[1]);

  if (system(buf) != 0) {
    fprintf(stderr, "ERR %s\n", buf);
    perror(NULL);
    return EXIT_FAILURE;
  }

  // FIXME O_DSYNC
  //       O_NDELAY [QNX special]
  //int fd = open(argv[1], O_RDWR | O_NOCTTY | O_SYNC);
  int fd = open(argv[1], O_RDWR | O_NOCTTY);

  if (fd < 0) {
    fprintf(stderr, "ERR Could not open device.\n");
    perror(NULL);
    return EXIT_FAILURE;
  }

  // give I/O privileges (e.g. manipulating with interrupt enable flag) to
  //   this thread
  if (ThreadCtl(_NTO_TCTL_IO, 0) == -1) {
    fprintf(stderr, "ERR ThreadCtl(_NTO_TCTL_IO...\n");
    perror(NULL);
    exit(EXIT_FAILURE);
  }

  struct sigevent event;
  event.sigev_notify = SIGEV_INTR;

  int int_com1;
  if ( (int_com1 = InterruptAttachEvent(COM1, &event,
        _NTO_INTR_FLAGS_TRK_MSK)) == -1 ) {
    fprintf(stderr, "ERR InterruptAttachEvent(COM1...\n");
    perror(NULL);
    return EXIT_FAILURE;
  }

  for (char first_iter = 1;;) {
#ifdef DEBUG
    fputs("Before InterruptWait() for COM1 [RX].\n", stdout);
#endif
    InterruptWait(0, NULL);
#ifdef DEBUG
    fputs("Woken up by COM1 interrupt.\n", stdout);
#endif

#ifdef DEBUG
    uint8_t tmp = in8(COM1_BASE_REG + SER_8250_REG_II);
    printf("REG_II: %d\n", tmp);
#else
    // read Interrupt Identification Register
    in8(COM1_BASE_REG + SER_8250_REG_II);
#endif

    // unmask the interrupt, so we can get the next event
    InterruptUnmask(COM1, int_com1);

    // TODO remove this hack around the problem with corrupted (???) COM1_IIR
    if (first_iter) {
      first_iter = 0;
      continue;
    }

    // determine what caused the interrupt
    //if ((tmp & SER_8250_IIR_NO_INT_PENDING)) continue;
#ifdef DEBUG
    fputs("Some interrupt pending.\n", stdout);
#endif
    //if (! (tmp & SER_8250_IIR_RX_AVAILABLE)) continue;

    // just for sure the 8250 driver does not get stuck if the data
    //   are not read
    in8(COM1_BASE_REG + SER_8250_REG_RX);

    if (InterruptDetach(int_com1) == -1) {
      fprintf(stderr, "ERR InterruptDetach(int_com1...\n");
      perror(NULL);
      return EXIT_FAILURE;
    }

    break;
  }

  uint64_t next_rising_edge_at;  // nanoseconds
  if (ClockTime(CLOCK_REALTIME, NULL, &next_rising_edge_at) == -1) {
    fprintf(stderr, "ERR ClockTime(...next_rising_edge_at\n");
    perror(NULL);
    return EXIT_FAILURE;
  }

  // schedule the first DTR rising edge to next nearest multiple of our
  // time period (always round up in division to avoid hardware uniform
  //   distribution of up/down rounding)
  next_rising_edge_at = ( (next_rising_edge_at + (DTR_PERIOD_NS -1)) /
      DTR_PERIOD_NS ) * DTR_PERIOD_NS;

  int int_sys_clock;
  if ( (int_sys_clock = InterruptAttachEvent(SYS_CLOCK, &event,
        _NTO_INTR_FLAGS_TRK_MSK)) == -1) {
    fprintf(stderr, "ERR InterruptAttachEvent(SYS_CLOCK...\n");
    perror(NULL);
    return EXIT_FAILURE;
  }

  // FIXME foundry27 does ClockAdjust() affects the period set by ClockPeriod() function?
  // I.e. the implementation of ClockPeriod is directly dependent only on
  // hardware cycle counters.

  // DTR generator
  for (;;) {
    InterruptWait(0, NULL);
    InterruptUnmask(SYS_CLOCK, int_sys_clock);

    // get adjusted (by ClockAdjust()) current system time
    uint64_t cur_time;
    if (ClockTime(CLOCK_REALTIME, NULL, &cur_time) == -1) {
      fprintf(stderr, "ERR ClockTime(...\n");
      perror(NULL);
      return EXIT_FAILURE;
    }

    if (next_rising_edge_at <= cur_time) {
      // schedule new rising edge
      next_rising_edge_at += DTR_PERIOD_NS;

      //out8(COM1_BASE_REG + SER_8250_REG_MC,
      //    in8(COM1_BASE_REG + SER_8250_REG_MC) | BIT0);

      int data;
      // see sys/dcmd_*.h files
      data = _CTL_DTR_CHG;  // definitely change it to value below
      data |= _CTL_DTR;     // set DTR bit(s)

      if (devctl(fd, DCMD_CHR_SERCTL, &data, sizeof(data), NULL)) {
        fputs("ERR setting DTR.\n", stderr);
        perror(NULL);
        return EXIT_FAILURE;
      }

#ifdef DEBUG
      fputs("FIRST  ", stdout);
      fsync(fileno(stdout));
      check_statuses(fd);
#endif

      // MSP430 needs at least 20ns pulse width to generate INT, but this
      //   empty place (caused by function calls, context switching
      //   etc.) acts like a delay for the DTR rising edge to fully rise,

      data = _CTL_DTR_CHG;
      data &= ~_CTL_DTR;

      if (devctl(fd, DCMD_CHR_SERCTL, &data, sizeof(data), NULL)) {
        fputs("ERR unsetting DTR.\n", stderr);
        perror (NULL);
        return EXIT_FAILURE;
      }

      //out8(COM1_BASE_REG + SER_8250_REG_MC,
      //    in8(COM1_BASE_REG + SER_8250_REG_MC) & ~BIT0);

#ifdef DEBUG
      fputs("SECOND ", stdout);
      fsync(fileno(stdout));
      check_statuses(fd);
#endif
    }
  }

  return EXIT_SUCCESS;
}
