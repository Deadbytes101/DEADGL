# DEADGL v11.0.1

Language docs sync cut.

## Changed

- README now mentions `deadgl grammar` and the `dgl.grammar` release artifact.
- PROOF now records `dgl.grammar` in the suite proof artifacts.
- NIGHTSHIFT now points at this v11.0.1 docs sync cut.

## No feature bump

This is a documentation and proof-language sync after v11.0.0. The language hardening feature was shipped in v11.0.0.

## Run

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\cut.ps1 -Version 11.0.1 -Publish
```
