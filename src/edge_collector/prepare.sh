#!/bin/sh

[ $# -eq 1 ] || {
  echo "USAGE: $0 <base_directory>"
  exit
}

[ -d "$1" ] || {
  echo "Directory $1 does NOT exist!" >&2
  exit 1
}

time="$(date '+%Y-%m-%d-%H%M%S')"
dir="$1/$time/"

mkdir "$dir" || exit $?
cat > "$dir"/topology.info <<HERE_DOC
# Topology specification (how many and which components, which
#   settings, which wiring...)
# measure: $time

ClockPeriod(): ---------39390ns

measure period: ---------130s
network load: 
system load: 
time synchronization: 
PTP/NTP daemon priority: ------------- -25
edge_generator priority: ------------- -28
FitKit calibration: -------none

!!!!!!!!!!!  WIRE TOPOLOGY  !!!!!!!!!!!
HERE_DOC
cat > "$dir"/results.raw <<HERE_DOC
# Results
# measure: $time

HERE_DOC

vim -O -c 'set paste' "$dir"/topology.info "$dir"/results.raw

# TODO -f is GNU-specific; use `cd; pwd; cd $OLDPWD' approach
"$(dirname "$(readlink -f "$0")")/gen_charts.sh" "$dir/"

echo "DONE with $dir/"
