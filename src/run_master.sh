#!/bin/sh

# <xpacne00@stud.fit.vutbr.cz> wrote this file. As long as you retain this
# notice you can do whatever you want with this stuff. If we meet some day,
# and you think this stuff is worth it, you can buy me a beer in return.
# Jan Pacner

case "$1" in
  ntp)
    echo "NOT prepared yet" ;;
  ptp)
    # be as verbose as possible
    ptpd2 -G -b vboxnet0 -C -DVfS ;;
  *)
    echo "USAGE: $0 ntp|ptp|???" ;;
esac
