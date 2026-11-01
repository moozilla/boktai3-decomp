# Defeat burst (08080494)

Tags: #scene-actors #matching

The concession-free guarded draft reaches 88.6%. The former 98.9% annotation
came from the pinned draft in b420c76c2. Removing its pins and barriers also
removed the particle table's register allocation, while leaving the annotation.

The six sparks draw consecutive samples from the same 1,024-entry table and
the eight script arguments now use the behavior view's parameter array. The
remaining particle setup holds the RNG cursor across calls where the ROM keeps
the lifetime of 20. The current candidate is 976 bytes against 980.
