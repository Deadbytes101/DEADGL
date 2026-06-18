# Connector note

The v1.2 branch currently contains the safe planning docs and a raw depth cube scene.

The C implementation must be committed by an environment that can write `src/*.c` directly and run the local toolchain. The current connector session rejected direct C source replacement, so main was reset back to the last stable v1.1 release-tooling state before any partial source commit could remain.

Do not merge `v1.2-geometry-pipe` until the C command parser supports the planned `camera`, `face`, and `wireface` commands and local validation passes.
