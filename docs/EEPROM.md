# EEPROM SDK source matches

Five routines (808 emitted bytes) match the EEPROM_V124 family using
**agbcc `-O1 -mthumb-interwork`**. B3 also contains the `EEPROM_V124` string
at `08602D20`. These are source-level and byte-exact identifications, not
inferences from the version string alone.

Sources: [laqieer/libgbabackup](https://github.com/laqieer/libgbabackup/blob/9597360b36d22df1ead5dd59566a3c8618cff6c0/src/eeprom.c)
(first structural comparison) and the more complete
[zeldaret/tmc reconstruction](https://github.com/zeldaret/tmc/blob/6fb6dfb4a7efbe24d0fd1dda5097af6131faacde/src/eeprom.c)
(read/write routines). Neither pinned repository has a root license file;
these adapted files retain their upstream licensing status, separate from MIT.

| Address | Emitted bytes | Source operation |
|---|---:|---|
| `08248634` | 128 | DMA3Transfer |
| `082486B4` | 176 | EEPROMRead |
| `08248778` | 352 | EEPROMWrite |
| `082488D8` | 88 | EEPROMCompare |
| `08248930` | 64 | EEPROMWrite1_check |

The final wrapper was hidden in the preceding function's trailing assembly
blob. It is now included in the same contiguous C unit and has its own reviewed
progress boundary. Four files cover these five routines; the existing configure
and write-one wrapper functions are not counted again.

The DMA helper masks the WAITCNT value with `F8FF`, merges the configuration's
wait-state bits, disables interrupts while DMA3 runs, and restores IME. The
read/write format serializes a variable-width address plus 64 data bits through
`0D000000`, an EEPROM hardware mapping, not relocatable ROM data. The read helper
skips four dummy bits and reverses the output halfword order. The write helper
counts video scanlines, wraps at 228, and returns `C001` on timeout. The compare
helper returns `8000` on the first mismatch; out-of-range requests return `80FF`.

Ordinary `-O2` and old_agbcc probes failed for the DMA/read/write bodies. `-O1`
matched DMA without source-shape changes. The upstream GCC cast-lvalue expressions
in the read helper and the current TMC timeout loop then matched the other two.
These results justify this flag locally; they do not imply game code generally
uses `-O1`. RAM configuration pointer `03006A9C` is inferred from the original
literal loads and its fields, with no added storage or ROM tables.
