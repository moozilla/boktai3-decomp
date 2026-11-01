# Defeat burst (08080494)

Tags: #scene-actors #matching

The concession-free guarded draft reaches 89.0%. The former 98.9% annotation
came from the pinned draft in b420c76c2. Removing its pins and barriers also
removed the particle table's register allocation, while leaving the annotation.

The six sparks draw consecutive samples from the same 1,024-entry table and
the eight script arguments now use the behavior view's parameter array. The
remaining particle setup holds the RNG cursor across calls where the ROM keeps
the vertical range of 20. The current candidate has the ROM's 980-byte extent
and 47 differing instructions after alignment. Both 08080494 and 0807D2E0
pass the low halfword of the actor ID to the defeat-record update.

The gated category-1/4 reaction at 0807D2E0 uses the same spread, six sampled
counts and angles. A shared inline emits them in order. Before restoring its
actor ID truncation, its guarded object was byte-for-byte unchanged by the
extraction. With both changes the stock score is 60.3%, with 603 differing
instructions after alignment and 431 when ignoring registers (previously 433).
