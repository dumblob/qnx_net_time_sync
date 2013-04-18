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
