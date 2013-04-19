awk 'BEGIN {i=1} {
  if ($0 ~ /^M/) {
    printf("%d %s ", i++, $2)
  }
  else if ($0 ~ /^S/) {
    print $2
  }
}' < results.raw > results.both
