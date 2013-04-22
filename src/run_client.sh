#!/bin/sh

# "THE BEER-WARE LICENSE" (Revision 42):
# <xpacne00@stud.fit.vutbr.cz> wrote this file. As long as you retain this
# notice you can do whatever you want with this stuff. If we meet some day,
# and you think this stuff is worth it, you can buy me a beer in return.
# Jan Pacner

#FIXME specify inbound + outbound latency in nsec (compesate e.g. sockets usage)
#  ptpd -l IN,OUT

case "$1" in
  ntp)
    echo "NOT prepared yet" ;;
  ptp)
    [ "$3" == 'recompile' ] && {
      make -C ./ptp/new/src clean
      make -C ./ptp/new/src
    }
    # be as verbose as possible
    ./ptp/new/src/ptpd2 -g -b "$2" -C -DVfS ;;
    #./orig/new/src/ptpd2 -g -C -DVfS -b en0 ;;
  *)
    echo "USAGE: $0 (ptp <eth_device> [recompile]|ntp)" ;;
esac
