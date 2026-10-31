# Round 7: Vorn defeat completion

Tags: #matching #monsters #vorn #080AEE08 #scope

`Vorn_UpdateDefeat` at `080AEE08` now matches its complete 984-byte extent in
ordinary C. The source uses the shared actor, route, animation, script, and
particle interfaces. It clears the entry response, emits the defeat burst,
dispatches the configured script, records the bestiary event, and installs idle
control. The previous guarded draft and its register-pinning marker are removed.

## Matching decision

The difficult late difference was a halfword zero in R4 instead of R5. Earlier
experiments proved that the local prepared-entry value must first reserve R4,
then lose it during global allocation after it has already been considered. A
shared entry-clear/sample-table work value creates that eviction naturally, but
its interaction with the six particle calls depends on the local burst values.

An inline function returning particle kind 2 expanded before the stack argument
stores and moved its first definition too early. A statement expression that
constructs the same typed kind through the assignment operation keeps that
definition at its stack argument. It has eight weighted references.

The packed spread receives an explicit pointer after X and Y are initialized.
Writing Z through a bitfield pointer would narrow the store to a halfword, losing
the ROM's whole-word update. The union's word view preserves the unused upper
half while setting the low half to 60. That leaves ten weighted references on
the spread pointer. The actor-position capture has eight references.

The resulting stock local allocation is kind R4 (priority 1318), spread R5
(1271), and origin R6 (1224). The shared work value then takes R4, and kind is
reallocated to R10. The prepared-entry value stays unallocated, so the control
reset retains its halfword zero in R5. No compiler overrides are part of the
source or verification.

## Verification

- `tools/check.py src/fn/sub_080AEE08.c --target-size 984 --json`: zero differing
  bytes, reviewed complete extent, stock compiler.
- Scope independently reports `MATCH`, 984 bytes.
- Target and candidate SHA-256:
  `49a8d351adbf1451e49c34a05a6ac8b6afbca13c48ad6bf4b2361699268945bf`.
- Formatting, whitespace, and staged PII checks pass. Aggregate progress files
  are left to the integrator.

## Scope gap

An identical scalar value supplied by an inline call and by a statement
expression can expand at different positions in a larger call. Allocation
weights alone did not explain this. The useful comparison is the first actual
definition, its subsequent stack-argument store, and the emitted position of
other argument computations. Early expansion of inline call arguments should
be surfaced alongside the allocation diagnosis.
