# Axe body contact

Tags: #matching #monsters #axe #080F75C0 #080FA830 #080F79A8

`Axe_OnBodyContact` at 080F75C0 matches all 340 bytes with the stock compiler.
It applies the shared contact reaction, records the first response, and when
armed with a timer above two disables the three attack and three body
colliders before entering reverse flicker. The previous provisional name is
promoted into the symbol table and its collider-setup caller is renamed.

The ordinary C loop retains a native attack stride and a captured body record
layout. Its per-iteration variable-length record type lets the compiler move
the actual stride setup at the required time. The post-tested index predicate
reads its by-value index through a borrowed reference; this preserves the
count-up loop and the original attack/body advancement order. There are no
register pins, asm barriers, compiler overrides, or new selectors.

Independent stock compilation and standalone linking verified zero differing
bytes across the reviewed 340-byte extent, SHA-256
`77e2cb55c6c56c676f1f25e4a35f2eccf60ce0ac73b3757cb356736af0980d8d`.
The renamed `Axe_InitColliders` caller at 080FA830 also remains byte-identical
across its 492 bytes, SHA-256
`722416c19968a8e7fbdc807db502f3f9b9ce8125476b55c39244386b58745d32`.
The orchestrator runs the combined ROM build during integration.

The private diagnostic had established two coupled causes: early stride
motion and first-versus-second loop-pass induction recognition. The captured
layout fixed stride motion; the real predicate index copy kept the induction
variable live through the first pass, and the post-tested loop coalesced that
copy without changing the original instruction count.

The same loop structure also transferred to `Axe_UpdateOrbitAttack` at
080F79A8, independently stock-verified across 1640 bytes and committed by the
parallel worker as `74a278e85b6a5dba3925a1615fdca739fe4bd659`.
