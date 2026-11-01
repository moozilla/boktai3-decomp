# NamedActor_SetMoveModeFromScript

The ROM compares the full script value of `m` against 2 and 4. The older draft
narrowed it to eight bits, so a value of 258 could incorrectly set bit 1. The
corrected guarded implementation uses the shared named object slot.

The 132-byte candidate scores 93.7%. Every instruction producing a live result
agrees after realignment. The original's unused zero in r6 at +0x3E, its
save/restore and alignment account for the remaining differences.
