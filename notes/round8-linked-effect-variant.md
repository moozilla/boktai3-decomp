# LinkedEffect_SetVariant

Tags: #matching #effects #compiler #round8

All 228 bytes at 0812A928 match with stock agbcc. Capturing the RNG table in a
statement macro gives its initialization enough weight to take r8. Declaring
the remaining-count value before the index mask settles their tie at r9 and
r10. Keeping the final sample's byte offset in the same storage as its index
preserves the in-place address calculation.
