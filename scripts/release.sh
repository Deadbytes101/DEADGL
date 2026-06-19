#!/usr/bin/env sh
set -eu

version="1.5.0"
tag="v$version"
dist="dist/deadgl-$version"
archive="dist/deadgl-$version-source.tar.gz"

rm -rf dist
mkdir -p "$dist"

make clean test
make clean sanitize
make

./build/deadgl prove examples/command_machine.dgl -o "$dist/command_machine.ppm" -p "$dist/command_machine.proof"
DEADGL_BENCH_LOOPS=200 scripts/bench.sh examples/command_machine.dgl

cp build/deadgl "$dist/deadgl-linux-x86_64" 2>/dev/null || cp build/deadgl.exe "$dist/deadgl-windows.exe"
cp build/benchmark.txt "$dist/benchmark.txt"
cp README.md MANIFESTO.md LICENSE PROOF.md "$dist/"
cp docs/RELEASE_V1.5.0.md "$dist/RELEASE_NOTES.md"

tar --exclude=.git --exclude=build --exclude=build-cmake --exclude=dist -czf "$archive" .

cat > "$dist/SHA256SUMS.txt" <<EOF
EOF
if command -v sha256sum >/dev/null 2>&1; then
  (cd "$dist" && sha256sum * > SHA256SUMS.txt)
  sha256sum "$archive" >> "$dist/SHA256SUMS.txt"
fi

printf '\nDEADGL local release cut complete.\n'
printf 'dist folder : %s\n' "$dist"
printf 'source tar  : %s\n' "$archive"
printf '\nOptional publish with GitHub CLI:\n'
printf 'gh release create %s %s/* %s --title "DEADGL %s" --notes-file docs/RELEASE_V1.5.0.md\n' "$tag" "$dist" "$archive" "$tag"
