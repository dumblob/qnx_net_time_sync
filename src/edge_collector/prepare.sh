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

HERE_DOC
cat > "$dir"/results.raw <<HERE_DOC
# Results
# measure: $time
HERE_DOC

vim "$dir"/topology.info
vim -c 'set paste' "$dir"/results.raw

echo "DONE with $dir/"
