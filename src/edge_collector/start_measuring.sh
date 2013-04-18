#!/bin/sh

# "THE BEER-WARE LICENSE" (Revision 42):
# <xpacne00@stud.fit.vutbr.cz> wrote this file. As long as you retain this
# notice you can do whatever you want with this stuff. If we meet some day,
# and you think this stuff is worth it, you can buy me a beer in return.
# Jan Pacner

# http://www.linuxquestions.org/questions/programming-9/serial-port-i-o-with-bash-730482/

# /dev/ttyUSB0
stty -F "$1" raw ispeed 2400 ospeed 2400 cs8 -ignpar -cstopb -echo
