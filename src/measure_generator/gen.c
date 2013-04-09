/**
 * periodical generator of rising edges on RS232 DTR port
 * Jan Pacner xpacne00@stud.fit.vutbr.cz
 * 2013-01-06 16:48:43 CET
 */

// TODO
//   disable System Management Mode (SMM) in BIOS else unpredictable 100us
//     interrupt latencies occur
//   a typical example is USB legacy support

//FIXME nsec2timespec()

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
#include <atomic.h>  // atomic_add() atomic_sub()

// interrupt vectors
#define SYS_CLOCK 0  // on x86: clock with resolution set by ClockPeriod()
#define COM1 4
#define COM2 3

//FIXME does 0.5ms brake the system?
#define CLOCK_PERIOD_NS 500000
#define DTR_PERIOD_NS   1000000000

// FIXME atomic_add(), atomic_sub() ensures atomicity
// this is critical section, but it is atomic byte value writable only
//   by one thread and read only by another thread (ISR) => no need to
//   use critical section guard mechanisms
// all variables shared with ISR must be volatile
static volatile unsigned char start_generation = 0;

typedef struct {
  int fd;
} no_page_area_t;

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

// ISR runs in the same context as the process but with its own stack
const struct sigevent* dtr_generator(void* area, int id) {
  id = id;
  static unsigned int tics = 0;

  if (start_generation) {
    // generate DTR every second
    if (tics == 0) {
      int data;
      int fd = ((no_page_area_t *)area)->fd;

      // see sys/dcmd_*.h files
      data = _CTL_RTS_CHG;  // definitely change it to value below
      //data |= _CTL_RTS;      // set RTS bit(s)
      data |= _CTL_DTR;      // set DTR bit(s)

      //FIXME should be fine with 200B stack which ISR has
      if (devctl(fd, DCMD_CHR_SERCTL, &data, sizeof(data), NULL)) {
        fprintf(stderr, "ERR setting DTR.\n");
        perror (NULL);
      }

#ifdef DEBUG
      check_statuses(fd);
#endif

      // this empty place (caused by function calls, context switching
      //   etc.) acts like a delay for the DTR rising edge to fully rise

      data = _CTL_DTR_CHG;
      data &= ~_CTL_DTR;

      if (devctl(fd, DCMD_CHR_SERCTL, &data, sizeof(data), NULL)) {
        fprintf(stderr, "ERR unsetting DTR.\n");
        perror (NULL);
      }

#ifdef DEBUG
      check_statuses(fd);
#endif

      tics = DTR_PERIOD_NS / CLOCK_PERIOD_NS;
    }
    else {
      --tics;
    }
  }

  return NULL;  // no further processing
}

#define SER_BASE_REG 0x2f8
#define REG_II       2
//#define REG_MS       6
//#define REG_RX       0
//#define REG_LS       5
#define IIR_MASK     7
//#define IIR_MSR      0
//#define IIR_THE      2
#define IIR_RX       4
//#define IIR_LSR      6
//volatile int serial_msr;  // saved contents of Modem Status Reg
//volatile int serial_rx;   // saved contents of RX register
//volatile int serial_lsr;  // saved contents of Line Status Reg
//struct sigevent *event = (struct sigevent *)arg;
//// determine the source of the interrupt
//// by reading the Interrupt Identification Register
//// in*() accesses device registers mapped using mmap_device_io()
//int iir = in8 (base_reg + REG_II) & IIR_MASK;
//// no interrupt -> no event
//if (iir & 1) return NULL;
//switch (iir) {
//  case IIR_MSR:
//    serial_msr = in8(base_reg + REG_MS);
//    return event; // wake up thread
//    break;
//  case IIR_THE:
//    break; // do nothing
//  case IIR_RX:
//    serial_rx = in8(base_reg + REG_RX); // note the character
//    break;
//  case IIR_LSR:
//    serial_lsr = in8(base_reg + REG_LS); // note the line status register
//    break;
//  default:
//}

const struct sigevent* rxd_handler(void* area, int id) {
  area = area;
  id = id;

  // FIXME II exchange with RX ???
  // react only on RXD
  int tmp = in8(SER_BASE_REG + REG_II) & IIR_MASK;

  if (tmp & 1 || !(tmp & IIR_RX))
    return NULL;  // no further processing

  // detaching this interrupt would result in a high code complexity
  //   => let this ISR do nothing instead
  static int first_run = 1;

  if (first_run) {
    atomic_add(start_generation, 1);
    first_run = 0;
  }

  return NULL;
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
    fprintf(stderr, "ERR Could not open device.\n");
    perror(NULL);
    return EXIT_FAILURE;
  }

  no_page_area_t *area = (no_page_area_t *)malloc(sizeof(no_page_area_t));
  // lock the memory shared by the interrupt handler and the process
  // http://www.qnx.com/developers/docs/6.3.2/neutrino/lib_ref/i/interruptattach.html#HandlerFunction
  mlock(area, sizeof(no_page_area_t));

  // To set up the serial port use stty from a command line:
  //   stty baud=9600 bits=8 par=none stopb=1 -ihflow -ohflow < /dev/ser1
  // The first 4 are fairly obvious; the -ihflow and -ohflow turn off
  //   support flow control using the hardware handshaking lines, and of
  //   course the < redirects so the correct port is set.
  // TODO use termios control structure instead
  #define BUF_SIZE 512
  char buf[BUF_SIZE];
  snprintf(buf, BUF_SIZE, "stty baud=460800 bits=8 par=none stopb=1 -ihflow -ohflow < %s", argv[1]);

  if (system(buf) != 0) {
    fprintf(stderr, "ERR %s\n", buf);
    perror(NULL);
    return EXIT_FAILURE;
  }

  // FIXME O_RDWR | O_NOCTTY | O_SYNC
  //       O_DSYNC
  //       O_NDELAY [QNX special]
  //area->fd = open(argv[1], O_WRONLY | O_NOCTTY | O_SYNC);
  area->fd = open(argv[1], O_RDWR | O_NOCTTY | O_SYNC);

  if (area->fd < 0) {
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

  if (InterruptAttach(SYS_CLOCK, dtr_generator, area,
      sizeof(no_page_area_t),
      _NTO_INTR_FLAGS_TRK_MSK) == -1) {
    fprintf(stderr, "ERR InterruptAttach(SYS_CLOCK...\n");
    perror(NULL);
    return EXIT_FAILURE;
  }

  if (InterruptAttach(COM1, rxd_handler, NULL,
      0,
      _NTO_INTR_FLAGS_TRK_MSK) == -1) {
    fprintf(stderr, "ERR InterruptAttach(COM1...\n");
    perror(NULL);
    return EXIT_FAILURE;
  }

  // idle forever (Ctrl+C will break it and kernel will clean all
  //   attached interrupts and memory)
  for(;;) sleep(1024);

  return EXIT_SUCCESS;
}
