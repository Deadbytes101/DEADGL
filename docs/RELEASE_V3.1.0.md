# DEADGL v3.1.0

DGB disassembler cut.

## Added

- `deadgl disasm file.dgb`
- deterministic DGB metadata output
- Makefile test coverage for disasm output

## Output

```text
DEADGL_DGB
version X.Y.Z
file scene.dgb
magic DEADGL_DGB_1
payload_bytes N
payload_lines N
```

## Changed

- Windows release packages `near_clip.disasm`

## Run

```sh
./deadgl pack examples/near_clip.dgl -o near_clip.dgb
./deadgl disasm near_clip.dgb
./deadgl unpack near_clip.dgb -o near_clip.unpack.dgl
```
