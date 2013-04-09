#!/bin/sh

#FIXME specify inbound + outbound latency in nsec (compesate e.g. sockets usage)
#  ptpd -l IN,OUT

case "$1" in
  ntp)
    echo "NOT prepared yet" ;;
  ptp)
    [ "$2" == 'recompile' ] && {
      make -C ./orig/new/src clean
      make -C ./orig/new/src
    }
    # be as verbose as possible
    ./orig/new/src/ptpd2 -g -C -DVfS ;;
    #./orig/new/src/ptpd2 -g -C -DVfS -b en0 ;;
  *)
    echo "USAGE: $0 ntp|ptp|???" ;;
esac
