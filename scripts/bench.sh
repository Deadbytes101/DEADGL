#!/usr/bin/env sh
set -eu

mkdir -p build
out="build/benchmark.txt"
scene="${1:-examples/command_machine.dgl}"
loops="${DEADGL_BENCH_LOOPS:-100}"

make >/dev/null
./build/deadgl run "$scene" -o build/bench_warmup.ppm >/dev/null

start=$(date +%s%N)
i=0
while [ "$i" -lt "$loops" ]; do
  ./build/deadgl run "$scene" -o build/bench.ppm >/dev/null
  i=$((i + 1))
done
end=$(date +%s%N)

ns=$((end - start))
per=$((ns / loops))
ms_int=$((per / 1000000))
ms_frac=$(((per / 1000) % 1000))

hash_line=$(./build/deadgl hash "$scene")

{
  echo "DEADGL BENCHMARK"
  echo "version $(./build/deadgl --version)"
  echo "scene $scene"
  echo "loops $loops"
  echo "total_ns $ns"
  echo "avg_ns $per"
  printf "avg_ms %d.%03d\n" "$ms_int" "$ms_frac"
  echo "hash $hash_line"
  echo "cc ${CC:-cc}"
  echo "cflags ${CFLAGS:-default}"
  echo "system $(uname -a 2>/dev/null || echo unknown)"
} | tee "$out"

echo "benchmark report: $out"
