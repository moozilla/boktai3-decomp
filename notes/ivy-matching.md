# Ivy (080ED914..080F0B8C): matching notes

Tags: #matching

Techniques that removed register pins or matched outright in this family. Each one
explains a specific ROM pattern, so check for the pattern before reaching for a pin.

* **Duplicated return paths raise a value's priority** (`sub_080F0910`). Each side of
  the final choice ends in its own `return Keep(actor, target);`. Cross-jumping merges the
  copies into one block in the output. Before that, the extra references raise the
  target's allocation priority above a short-lived candidate, which fixed an r6/r7 swap.
  The same rewrite fixed the ROM's per-case `cmp; bne fail; b found` layout: with a
  shared tail, the cases' compares got merged.
* **A call's memory arguments are loaded at the call** (`calls.c`,
  `precompute_register_parameters`). When the ROM loads arg0 last (`ldr r0, [rX, #off]`
  just before the `bl`), the other arguments were already in variables. Assign them to
  locals before the call.
* **Inline functions own a frame** (`integrate.c`). A store through a register that
  holds `sp+N`, set up well before the store, is the frame-base pseudo of an inlined
  function whose locals start at `sp+N`. Move the block holding those locals (here a
  CpuFill32 temporary) into an inline function.
* **A bitfield store leaves a zero behind.** `movs rX, #0` emitted just before a byte or
  halfword store of a constant is the `& 0` mask of a full-width bitfield insertion. Later
  stores of 0 reuse that register. `struct { u8 unknown[7]; u8 mode : 8; }` reproduced it.
* **Inline parameters are copied at entry when the argument is a constant.** If the ROM
  materialises one argument (3) before an address computation and the zeros only at their
  stores, the zeros are literals inside the helper rather than parameters
  (`ResetState(actor, 3)`, not `SetState(actor, 3, 0, 0)`).
* **Combine refolds `a - (b - 256)`.** To keep the ROM's `adds b, #-256; subs`, return
  `b - 256` from an inline (`WrapCounter`, as in `sub_0809B00C`).
* **Stack slot order:** BLKmode objects (u8 arrays, odd-sized or byte-aligned structs)
  and volatile locals get their slots at declaration. Address-taken scalars and
  register-sized structs get theirs at the end (ADDRESSOF purge), in insn order of
  their first address use.
