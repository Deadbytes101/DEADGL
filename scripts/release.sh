#!/usr/bin/env sh
set -eu

version=$(sed -n 's/^#define DGL_VERSION "\([^"]*\)".*/\1/p' include/deadgl.h | head -n 1)
tag="v$version"
dist="dist/deadgl-$version"
archive="dist/deadgl-$version-source.tar.gz"
notes="docs/RELEASE_V$version.md"

rm -rf dist
mkdir -p "$dist"

make clean test
make clean sanitize
make

sh scripts/verify_render_truth.sh
sh scripts/golden.sh
sh scripts/dgb_parity.sh

./build/deadgl prove examples/command_machine.dgl -o "$dist/command_machine.ppm" -p "$dist/command_machine.proof"
./build/deadgl prove examples/near_clip.dgl -o "$dist/near_clip.ppm" -p "$dist/near_clip.proof"
./build/deadgl inspect examples/near_clip.dgl > "$dist/near_clip.main.inspect"
./build/deadgl-inspect examples/near_clip.dgl > "$dist/near_clip.inspect"
DEADGL_BENCH_LOOPS=200 scripts/bench.sh examples/command_machine.dgl

cp build/deadgl "$dist/deadgl-linux-x86_64" 2>/dev/null || cp build/deadgl.exe "$dist/deadgl-windows.exe"
cp build/deadgl-inspect "$dist/deadgl-inspect-linux-x86_64" 2>/dev/null || cp build/deadgl-inspect.exe "$dist/deadgl-inspect-windows.exe"
cp build/benchmark.txt "$dist/benchmark.txt"
cp README.md MANIFESTO.md LICENSE PROOF.md "$dist/"
if [ -f "$notes" ]; then cp "$notes" "$dist/RELEASE_NOTES.md"; else cp docs/RELEASE_V2.0.0.md "$dist/RELEASE_NOTES.md"; fi

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
printf 'gh release create %s %s/* %s --title "DEADGL %s"\n' "$tag" "$dist" "$archive" "$tag"
