# Perf

Run the render loop test:

```sh
scripts/bench.sh
```

Run more loops:

```sh
DEADGL_BENCH_LOOPS=1000 scripts/bench.sh examples/command_machine.dgl
```

Report file:

```text
build/benchmark.txt
```

The report includes version, scene, loops, time, hash, compiler, flags, and system.
