# DEADGL Road to v10

DEADGL stays a CPU framebuffer command machine.

No GPU wrapper. No engine layer. No scene graph. No asset-first trap. The source is the machine.

```text
.dgl command stream -> parser -> framebuffer -> ppm -> proof hash
```

## Law

- One command changes memory.
- One render produces bytes.
- One proof records the bytes.
- A version is not real until it builds, tests, renders, hashes, and ships.
- Features that hide state are rejected.
- Features that make the pipe visible are allowed.

## v2 line: command machine hardening

### v2.1.0: source split

Break the current CLI mass into smaller native C units without changing render output.

Target files:

```text
src/deadgl_math.c
src/deadgl_scene.c
src/deadgl_script.c
src/deadgl_render_cmd.c
```

Rules:

```text
hashes must not move
commands must not change behavior
CLI must stay the same
```

### v2.2.0: golden proof tests

Add checked proof fixtures for core scenes:

```text
cube
command_machine
near_clip
```

### v2.3.0: parser diagnostics

Make parse failures sharper:

```text
bad command
bad arity
bad integer
bad float
bad color
bad vertex index
```

### v2.4.0: deterministic script audit

Add `deadgl audit scene.dgl`:

```text
commands
primitives
mesh refs
unknown count
estimated draw ops
```

## v3 line: machine language

### v3.0.0: bytecode command stream

Add a compact binary command format:

```text
.dgl text -> .dgb byte stream -> renderer
```

Text stays first. Binary exists for proof, speed, and tiny machines.

### v3.1.0: disassembler

Add:

```text
deadgl disasm file.dgb
```

### v3.2.0: bytecode proof parity

The same scene rendered from text and bytecode must hash the same.

## v4 line: visual terminal

### v4.0.0: software console overlay

Add tiny pixel text drawn by DEADGL itself:

```text
text x y color message
```

### v4.1.0: inspect overlay scene

A scene can draw its own command stats into the framebuffer.

## v5 line: tiny editor path

### v5.0.0: DEADPAD seed

A minimal `.dgl` editor prototype. Native. No framework. No bloat.

```text
open scene
edit command lines
render preview
write file
```

## v6 line: interactive command machine

### v6.0.0: live command shell

A REPL that edits framebuffer state directly:

```text
deadgl shell
> canvas 640 360
> clear #101018
> line 0 0 100 100 #ffffff
> save out.ppm
```

## v7 line: software acceleration

### v7.0.0: tile renderer

Split framebuffer into tiles for cache-local drawing. Output must remain deterministic.

## v8 line: platform metal

### v8.0.0: native window viewer

A tiny viewer for Windows first. The renderer still draws CPU memory. The window only displays bytes.

## v9 line: self-hosted scene toolchain

### v9.0.0: scene pack format

Bundle scenes, proofs, and hashes into a small archive format.

## v10 line: DEADGL system

### v10.0.0: command machine suite

Ship the complete native suite:

```text
deadgl          render/prove/hash/inspect/audit/shell
deadgl-inspect  standalone inspection
deadpad         tiny scene editor
deadview        native framebuffer viewer
```

v10 is not a feature number. v10 is the point where DEADGL becomes a small graphics system.
