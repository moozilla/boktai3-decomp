# Push20 Astra 081E013C escalation

Branch `codex/push20-astra-e013c` from `0b4492d`; owns only `sub_081E013C`.
No match accepted, zero new bytes. Best safe candidate remains the inherited
`wip/e013c-sol-best.c`, 700 bytes versus the 696-byte native span. The first
object pointer is rematerialized at `s+0xe8` instead of retained in r5 from
`s+0x78`, the initial offset temporaries differ in registers, and E000 setup
has an extra move.

Ten checks tried integer-address first-pointer arithmetic, incrementing the
first pointer, a typed first object with field at 0x70, deriving E002 from an
E000 local, unsigned offsets, an unbound register first-pointer hint, separate
size initialization/local declaration order, 32-bit bit constants, a scoped
halfword destination/value, and inline initial store helpers. None yielded a
useful improvement. Deriving E002 from E000 reduced the output to 692 bytes
but replaced the required literal and offset setup with derived constants;
it is not considered a better candidate. Incrementing `first` produced 728
bytes and substantially different allocation. Other tests retained the
700-byte baseline differences. Source/diff files are ignored in `wip/` with
prefix `e013c_`; inherited diff is `build/e013c-sol.diff`.

No permuter run: these checks did not establish a productive fresh hypothesis.
No unsafe or uninitialized reads, bound registers, asm, volatile operations,
artificial branches, other function edits, shared headers, or data changes.

Validation: complete `build/venv/bin/python tools/build.py` printed
`build/boktai3.gba: OK`. Only this note is committed.
