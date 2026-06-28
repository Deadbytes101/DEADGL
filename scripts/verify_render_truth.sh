#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT"

make all

mkdir -p build

build/deadgl run examples/raster_truth.dgl -o build/raster_truth.verify.ppm
build/deadgl prove examples/raster_truth.dgl -o build/raster_truth.verify.proof.ppm -p build/raster_truth.verify.proof
build/deadgl hash examples/raster_truth.dgl > build/raster_truth.verify.hash
build/deadgl inspect examples/raster_truth.dgl > build/raster_truth.verify.inspect

test -s build/raster_truth.verify.ppm
test -s build/raster_truth.verify.proof.ppm
test -s build/raster_truth.verify.proof
test -s build/raster_truth.verify.hash
test -s build/raster_truth.verify.inspect

grep -q '^DEADGL_PROOF$' build/raster_truth.verify.proof
grep -q '^hash ' build/raster_truth.verify.proof

echo render truth ok
