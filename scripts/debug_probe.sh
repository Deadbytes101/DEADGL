#!/usr/bin/env sh
set -eu

mkdir -p build
report="build/debug_report.txt"

{
  echo "DEADGL DEBUG PROBE"
  echo "date $(date -u +%Y-%m-%dT%H:%M:%SZ)"
  echo "system $(uname -a 2>/dev/null || echo unknown)"
  echo "cc ${CC:-cc}"
  echo
  echo "VERSION"
  make clean >/dev/null 2>&1 || true
  make >/dev/null
  ./build/deadgl --version
  echo
  echo "TEST"
  make test
  echo
  echo "PROOF"
  ./build/deadgl prove examples/command_machine.dgl -o build/debug_command_machine.ppm -p build/debug_command_machine.proof
  cat build/debug_command_machine.proof
  echo
  echo "HASH"
  ./build/deadgl hash examples/command_machine.dgl
  echo
  echo "BYTES"
  wc -c build/debug_command_machine.ppm build/debug_command_machine.proof
  if command -v sha256sum >/dev/null 2>&1; then
    sha256sum build/debug_command_machine.ppm build/debug_command_machine.proof
  fi
} | tee "$report"

echo "debug report: $report"
