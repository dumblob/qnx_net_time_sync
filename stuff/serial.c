// man 4 tty_ioctl
// DTR data terminal ready (1 if sending data; 0 if nothing should be read)
// DE-9 (== 9-pin RS-232) pinout
//   http://techpubs.sgi.com/library/dynaweb_docs/hdwr/SGI_EndUser/books/3900_UG/sgi_html/figures/pinouts.serial.gif
// QNX serial DTR
// linux serial DTR
//   http://stackoverflow.com/questions/12919644/pwm-signal-out-of-serial-port-with-linux
//   #include <termios.h>
//   http://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
//   http://www.unix.com/programming/29151-how-set-dsr-pin-using-c-code.html
//   https://groups.google.com/forum/?hl=en&fromgroups=#!topic/comp.unix.programmer/L6lLPYJ-7X4
// http://www.qnx.com/developers/docs/qnx_4.25_docs/qnx4/user_guide/chardev.html
// http://www.instructables.com/id/Serial-Controlled-Variable-Speed-Motor/

/*
  http://www.openqnx.com/newsgroups/viewtopic.php?f=5&t=9099

  qnx_ioctl(), look at the ioct header file there is a description of which
  bit does what.
  Beware that changing control line is asynchronous to rx/tx operation.
  write(...)
  ioctl()...
  The ioctl may actually flip control in the middle of the transfer requested
  by write.
  You need to make sure buffer are flushed and/or use unbuffered io.
*/

/*
  http://www.openqnx.com/newsgroups/viewtopic.php?f=13&t=13557

  int bitset;
  ioctl(fd, TIOCMGET, &bitset);
  ...will get you the bits set for that serial port (assuming you opened it
  first). Check out sys/ioctl.h for a list of the bits (TIOCM_DTR is the DTR
  bit). You can also use TIOCMSET to set the modem bits.
*/

/*
http://community.qnx.com/sf/discussion/do/listPosts/projects.community/discussion.community.topc12446?_message=1364719554191
*/
#include <devctl.h>
#include <sys/dcmd_chr.h>

// For "open()"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// For Errors
#include <stdlib.h>
#include <stdio.h>

int check_statuses(int fd) {
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
  return(1);
}

int main(int argc, char* argv[]) {
  int data = 0, fd, i;
  int rts = -1, dtr = -1;

  for (i = 1; i < argc; i++) {
    if      (!strcmpi(argv[i], "+RTS"))
      rts = 1;
    else if (!strcmpi(argv[i], "-RTS"))
      rts = 0;
    else if (!strcmpi(argv[i], "+DTR"))
      dtr = 1;
    else if (!strcmpi(argv[i], "-DTR"))
      dtr = 0;
  }

  if (rts != -1) {
    data |= _CTL_RTS_CHG;
    if(rts) data |= _CTL_RTS;
  }

  if (dtr != -1) {
    data |= _CTL_DTR_CHG;
    if(dtr) data |= _CTL_DTR;
  }

  if (data) {
    if (devctl (0, DCMD_CHR_SERCTL, &data, sizeof(data), NULL)) {
      fprintf(stderr, "Error setting RTS.\n");
      perror (NULL);
      exit(EXIT_FAILURE);
    }
  }

  check_statuses(0);
  return 0;
}
