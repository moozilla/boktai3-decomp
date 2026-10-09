# MGS/GCL matching round 2

Sol 6.1 worker; branch `codex/mgs-gcl-round2` from `8b00059` in the same
isolated checkout. No external C copied and no symbol names changed.

| New target | Bytes | Source variants | Matching observation |
|---|---:|---:|---|
| `0821AE04` | 204 | 2 | Little-endian inline reader; final addition order |
| `0821B938` | 292 | 3 | Direct case returns preserve operand-register mutation |
| `082255BC` | 100 | 5 | Explicit unsigned 16-bit wrapping counter arithmetic |
| `0821B4C8` | 120 | 1 | Reference descriptor-to-memory write wrapper |
| `0821B540` | 124 | 1 | Reference descriptor-to-memory read wrapper |
| `0821B5BC` | 128 | 2 | Byte pointer to stack reference preserves pointer register |
| `0821B63C` | 116 | 2 | Same pointer-local pattern for alternate memory read |
| `0821B3E4` | 148 | 1 | Adapted our matched B2B0 descriptor decoder into writer |
| `0821A340` | 168 | 1 | 392-byte buffer struct and message-record copies |
| `0821B34C` | 152 | 5 | Struct arrays reproduce base-first address-add operands |
| `0821A3E8` | 108 | 2 | Explicit first bound test and count reload before inner scan |

Total: 11 functions, 1,660 native bytes.
Full findings and B3/MGS differences are appended to `docs/MGS_GCL_MATCHING.md`.

Retained candidates: trap `08225624` has reconstructed static record operations but zeroing
loops either keep a live pointer in an extra high register or emit an extra
counter; its best natural variants remain untracked. A 60-second, two-worker
permuter pass against the pointer-loop candidate improved score 1,840 to
1,110 without a match. Its best output is retained as
`wip/sub_08225624_permuted_1110.c`. Typed load
`0821B20C` remains untracked: struct arrays fix address-add operand order, but
boolean normalization makes the compiler save the index in another register.
Three-component vector candidates from round 1 were not revisited because
larger successful functions were available.

Validation: all eleven additions passed `tools/check.py`; final complete ROM
build printed `build/boktai3.gba: OK` before commits. The parent runs the
combined 35 screenshot comparisons; no new worker screenshot run was started.
