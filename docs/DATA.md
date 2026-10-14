# Decoded data

The goal is a ROM whose data region is rebuilt from editable source files, the
same way the code region is rebuilt from C. This page describes the pipeline,
the policy for what is committed, and the formats decoded so far.

## Pipeline

* `data/manifest.csv` lists decoded ranges: `name,start,end,format,file,description`.
* `tools/datafmt/<format>.py` decodes a range to a source file (`decode`) and
  encodes the file back to items (`encode`): sized integers, and ROM pointers
  that `tools/symbolize.py` writes as symbols, so decoded tables stay movable.
* `tools/symbolize.py` emits each manifest range from its source file instead
  of `.incbin` slices. It refuses a range whose labels fall inside an item, or
  that leaves a detected pointer word unencoded.
* `tools/datafmt/check.py` checks that every entry re-encodes to the original
  bytes; `check.py --extract NAME` (re)writes an entry's file from the ROM.
  `make` rebuilds `gen/data.s` when a data file changes, and the final SHA-1
  check is the end-to-end test. Run `make shifttest` after adding a format
  (passed for `keyed_callbacks`, 2026-10-14).

To add a format: write the module, add the manifest row, run
`check.py --extract NAME`, then `make` and `make shifttest`.

## Policy

* **Committed:** structured tables whose content is code-like facts (keys,
  indices, counts, flags, pointers to code or data), as decomp projects commit
  data tables as source. Unknown fields stay as raw values in their columns;
  nothing is dropped, and decode/encode must be byte-exact.
* **Not committed:** graphics, palettes, maps, audio, text and any other
  creative asset. Those formats extract to the ignored `build/` tree from the
  user's own ROM, for viewers and editors to load.
* Names in decoded files are provisional unless backed by evidence, as in
  `symbols/`. Informational columns (for example a callback's current function
  name) are ignored by `encode`, so renames never break a build.

## Formats

| Format | Range | File | Notes |
|---|---|---|---|
| `keyed_callbacks` | `08603300`–`08604998` | `data/actor_registry.csv` | 722 sorted `(key, callback)` records and a zero sentinel. The script `chara` command binary-searches it (`08225844`) and calls the callback. |

### Actor registry keys are string hashes

`sub_08219BD8` hashes a NUL-terminated string as
`h = ((h << 5) | (h >> 11)) & 0xFFFF; h = (h + c) & 0xFFFF` per byte, the
scheme of MGS's GCL `GV_StrCode` without its position term. Registry keys are
spread uniformly over 16 bits, consistent with being such hashes of the
original actor names, which the script compiler hashed offline (the names are
not in the ROM). With 722 keys, a random string matches some key about 1.1% of
the time, so a candidate name needs independent evidence: hashing the ROM's
own strings gives 167 such collisions, including the message speaker tag
`HAKUSYAKU` ("count"), whose key belongs to the engine callback at `08003A68`, not to an actor. Treat any
name recovered by hashing as a hypothesis until a second fact (the callback's
behaviour, a matching family of names) supports it.
