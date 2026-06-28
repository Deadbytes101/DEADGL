# Final gate

A DEADGL cut is not final because a tag exists. It is final only when the machine proof passes.

Run on Windows before cutting a release:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\final_gate.ps1
```

The gate checks:

- `include/deadgl.h` exposes the release version
- `CMakeLists.txt` uses the same version
- `tests/test_deadgl.c` expects the same runtime version
- release notes exist for `docs/RELEASE_V<version>.md`
- `make clean test` passes
- `build/deadgl.exe --version` matches the release version
- golden scene hashes remain locked
- parser diagnostics remain locked
- DGB pack/disasm/unpack parity remains locked
- `examples/raster_truth.hash` still matches the freshly rendered hash

If any step fails, the cut is not real yet.
