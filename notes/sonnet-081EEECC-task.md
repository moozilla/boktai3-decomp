# Task 081EEECC worker notes

Matched: sub_081F3720, sub_081F3960, sub_081F3AE4.

Not matched (drafts saved as notes/sonnet-*.draft.c.txt, not compiled):
- sub_081F2A08 (~79%): frame/structure right; loop 1 uses two induction pointers (link ptr r7, field ptr r4) and
  register assignment (e=r6, pos=r5, to=r7) needs pinning; remaining diffs are param copy order (from=r8, style=sb).
- sub_081F2208 (~72%): stack layout right (zero 0x1c, position 0x20, size 0x28, offset 0x30, ptrs 0x38-0x40); e wants r7 but
  allocator gives r6 (size ptr takes r6 in original). Return type s32 with no return (epilogue pops r1).
- sub_081EEECC (~66%): offsets known (flagA 0x304, flagB 0x34c, counter 0x5a0, shake 0x5e6/0x5e8); table pointer is spilled
  to [sp+0x44] in the original and shakeX/shakeY addresses are held in r6/r5.
