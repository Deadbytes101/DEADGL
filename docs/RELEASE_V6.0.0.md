# DEADGL v6.0.0

Live command shell cut.

## Added

- `deadgl shell -o out.ppm`
- `deadgl shell scene.dgl -o out.ppm`
- stdin command stream rendering
- file replay shell rendering
- Makefile coverage for stdin shell and file shell

## Changed

- Windows release packages `shell.ppm`
- Windows release packages `shell_file.ppm`
- Windows release packages `shell.stdin.dgl`

## Rule

```text
The shell is a command stream.
It writes plain DGL into the existing renderer.
No hidden render state.
```

## Run

```text
deadgl shell -o shell.ppm
deadgl shell scene.dgl -o shell_file.ppm
```
