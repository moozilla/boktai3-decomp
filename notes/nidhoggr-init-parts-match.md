# Nidhoggr_InitParts (081940F8)

Tags: #matching #nidhoggr #cse #reload

Matched all 904 bytes in ordinary C. The stock translation-unit check also matches
the full 25,420-byte range starting at 081915E0.

The three ranges starting at colliders 2, 10 and 18 share a body attribute (0x120)
and contact callback. Capturing both at function entry lets reload place each
constant directly in its argument register after the preceding arguments. Literal
arguments in the last range put both loads too early.

The last pair initializes the time palette on the iteration that decrements the
counter below zero. Keeping its palette-index store inside that loop lets CSE share
the flags store's halfword zero. The last flags update retains the ROM's dead
`movs r6, #0` without the previous goto or single-pass wrapper.
