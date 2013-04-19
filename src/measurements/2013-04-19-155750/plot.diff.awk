awk 'BEGIN {i=1} {
  if ($0 ~ /^M/) {
    master = $2
  }
  else if ($0 ~ /^S/) {
    printf("%d %d\n", i++, $2 - master)
  }
}' < results.raw > results.diff
