# DEADGL v10.0.0

Command machine suite cut.

## Added

- `deadgl suite scene.dgl -o report`
- suite report output
- suite render proof image beside the report
- Makefile coverage for suite report and suite-rendered PPM

## Suite

The suite now ships these tools together:

```text
deadgl          render / prove / hash / inspect / audit / shell / pack / scenepack / suite
deadgl-inspect  standalone scene inspection
deadpad         plain-text scene editor seed
deadview        native PPM viewer seed
```

## Release artifacts

- `command_machine.suite`
- `command_machine.suite.ppm`
- `deadgl-windows.exe`
- `deadgl-inspect-windows.exe`
- `deadpad-windows.exe`
- `deadview-windows.exe`

## Rule

```text
v10 is not a number bump.
It is a packaged command machine suite with proof artifacts.
No hidden render state.
No GPU wrapper.
No scene graph.
```

## Run

```sh
./deadgl suite examples/command_machine.dgl -o command_machine.suite
```
