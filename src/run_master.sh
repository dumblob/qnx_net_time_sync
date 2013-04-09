#!/bin/sh

case "$1" in
  ntp)
    echo "NOT prepared yet" ;;
  ptp)
    # be as verbose as possible
    ptpd2 -G -b vboxnet0 -C -DVfS ;;
  *)
    echo "USAGE: $0 ntp|ptp|???" ;;
esac
