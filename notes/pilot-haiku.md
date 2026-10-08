# pilot-haiku notes

- 081041DC: not matched/skipped. Body matches with `*(u16 *)(obj + 0x156) |= flags;`, but the 18-byte
  literal-like block at 0x081041F0 that follows it is dropped when the C unit replaces the function, so
  every later ROM byte shifts (first diff reported at 0x08001482).
- 08108A98: skipped. Trailing data label `_08108AA4` is referenced from data (gUnk_08603300), so the
  link fails with an undefined reference once the function is C.
- 081009F4: skipped. Its address is used by data, so the link fails with undefined `sub_081009F4__addr`.
