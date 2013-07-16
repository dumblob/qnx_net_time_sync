#!/bin/sh

[ $# -eq 1 ] || {
  echo "USAGE: $0 <dir-with-results.raw>"
  exit
}

# TODO -f is GNU-specific; use `cd; pwd; cd $OLDPWD' approach
bdir="$(dirname "$(readlink -f "$0")")"
res_dir="$1"
res_file="$res_dir"/results.raw
topology_file="$res_dir"/topology.info

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
period="$(awk '/^measure period/ { print $3 }' < "$topology_file")"
[ -z "$period" ] && period="1 second"

substitute_markers() {
  # TODO -i is GNU-specific (i.e. non-POSIX)
  sed -r -i "s/[{]{3}F[}]{3}/$freq/" "$1"
  sed -r -i "s/[{]{3}N[}]{3}/$measure/" "$1"
  sed -r -i "s/[{]{3}PER[}]{3}/$period/" "$1"
}

rm -f "$res_dir"/stats || exit 1

MYPWD="$PWD"
ret=0
for kind in 'both' 'diff' 'sampling_period'; do
  cd "$MYPWD"

  "$bdir"/gen_stats.py $kind "$res_dir" "$freq" > \
    "$res_dir/results.$kind" || {
    echo "ERR gen_stats.py \"$kind\"" >&2
    ret=1
    continue
  }

  cp -f "$bdir/plot.${kind}.template" \
    "$res_dir/plot.${kind}" || {
    echo "ERR cp \"$kind\"" >&2
    ret=2
    continue
  }
  cp -f "$bdir/plot.${kind}.filtered.template" \
    "$res_dir/plot.${kind}.filtered" || {
    echo "ERR cp filtered \"$kind\"" >&2
    ret=3
    continue
  }

  cd "$res_dir"
  substitute_markers "plot.${kind}"
  substitute_markers "plot.${kind}.filtered"
  gnuplot "plot.${kind}" || {
    echo "ERR gnuplot \"$kind\"" >&2
    ret=4
    continue
  }
  gnuplot "plot.${kind}.filtered" || {
    echo "ERR gnuplot filtered \"$kind\"" >&2
    ret=5
    continue
  }
done

exit $ret
