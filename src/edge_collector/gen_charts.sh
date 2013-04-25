#!/bin/sh

[ $# -eq 1 ] || {
  echo "USAGE: $0 <dir-with-results.raw>"
  exit
}

# TODO -f is GNU-specific; use `cd; pwd; cd $OLDPWD' approach
bdir="$(dirname "$(readlink -f "$0")")"
res_dir="$1"
res_file="$res_dir"/results.raw

[ -d "$res_dir" ] || {
  echo "Directory does not exist: $res_dir" >&2
  exit 1
}

[ -e "$res_file" ] || {
  echo "No $res_file found." >&2
  exit 1
}

freq="$(awk '/^current freq/ { print $3 }' < "$res_file")"
measure="$(awk '/^# measure/ { print $3 }' < "$res_file")"

substitute_markers() {
  # TODO -i is GNU-specific (i.e. non-POSIX)
  sed -r -i "s/[{]{3}F[}]{3}/$freq/" "$1"
  sed -r -i "s/[{]{3}N[}]{3}/$measure/" "$1"
}

rm -f "$res_dir"/stats || exit 1

for kind in 'both' 'diff' 'sec'; do
  "$bdir"/gen_stats.py $kind "$res_dir" "$freq" > \
    "$res_dir/results.$kind" || exit 1

  cp -f "$bdir/plot.${kind}.template" "$res_dir/plot.${kind}" || exit 1
  cd "$res_dir"
  substitute_markers plot.$kind
  gnuplot plot.$kind || exit 1
  cd "$OLDPWD"
done
