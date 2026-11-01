# Sword orbit

Tags: 080F52C0 #sword #depin

`Sword_BeginTargetOrbit` matches all 364 bytes without its two register pins
or its barrier. Keeping the signed turn in a halfword temporary lets agbcc
retain the negation and copy before the random direction check.
