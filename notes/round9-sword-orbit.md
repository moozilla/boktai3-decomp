# Sword orbit

Tags: 080F52C0 #sword #depin

`Sword_BeginTargetOrbit` matches all 364 bytes without its two register pins
or its barrier. Keeping the signed turn in a halfword temporary lets agbcc
retain the negation and copy before the random direction check.

Tags: 080F5F24 #sword #depin

`Sword_BeginRestOrbit` also matches without pins or a barrier. The shared
direction calculation preserves the subtraction from the one used for the
sprite byte and the random bit mask. Both orbit routines pass an exact
extent check with that helper.
