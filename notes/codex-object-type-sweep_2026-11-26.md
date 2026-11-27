# Object type recovery across the six object directories

Tags: #matching #types #tooling #phase3

The sweep covers monsters, bosses, NPCs, objects, effects, and scenes. The first
compiler-measured catalog reported 224 families and 1,220 embedded-type
observations. The rescan after the first landing pass reported 186 families
and 1,216 embedded-type observations. Both scans used stock agbcc layouts and
ROM load evidence.

## Recovered storage

| View | Evidence and accepted units |
|---|---|
| Packed contact vector | The collider arguments, effect vectors, and scripted positions use the same aligned eight-byte storage. The first pass converted 45 units. Qualified member paths recovered 14 more in the rescan. |
| ContactPosition | Existing position type adopted for aligned private movement, target, and particle vectors. The first pass converted 38 units. |
| Packed contact halfwords | The pair and tail views occupy one aligned word. The high halfword is named where a caller uses it. All 23 units converted. |
| Packed collider extents | Twelve callers take the address of a whole word while also assigning individual halfwords. Their shared eight-byte view keeps the addressable `xy` and `zAndPad` members. |
| Animated particle configuration | Fixed indexes in the ImpactBurst emitters select the fields measured in the existing quest-barrier configuration. All nine units and 18 dependents match. |
| Scene actor allocation | The alternate return declarations at four Actor_Allocate call sites identify the shared 0x384-byte allocation. A checked union holds the lifecycle, story, and Otenko views and the state controller prefix. Three story handlers adopted StateActor. |
| Ratatosk orb and distance storage | The relocation union and the four distance entries use the same 0x60 bytes at 0xDF0. The generated layout preserves both interpretations and the anchor at 0x36C within the 0xF30 record. All three source units and header dependents match. |

The Otenko turn handler (081CFB54) uses the typed OtenkoActor view. The initial
conversion omitted the shifts at 081CFB86 and 081CFB88 because the shared
callback returns a word. Keeping the handler's test on the low byte reproduced
both shifts and the full original unit. The other Otenko views still expose
private tag, projection, playback, and motion layouts. The rising handler
(081CF9E8) reaches a measured nested position but still changes stock code.

The Ratatosk overlay uses the typed SunOrb_GetPositionFromManager declaration
at the distance call sites. Those sites write the distance and target distance
into separate fields. The relocation routine retains the orb targets and the
selected indices at the end of the fourth target slot.

## Tool behavior

The scanner records typed callee returns and follows a renamed embedded slot
when a typed handoff identifies the parent and the measured child fields agree.
The merger can lift equivalent by-value private children, forward-declare
pointer targets, and fill an opaque span supported by independent typed calls.
It preserves explicit overlapping alternatives, measures qualified aggregate
paths, and maps literal array accesses to measured scalars. Alignment,
whole-byte bitfields, tagged typedefs, and positional initializers now survive
the corresponding checks. Adopting a larger existing pointer view requires
unchanged alignment and an audit for by-value storage and stride.

The mechanical passes preserve an original address cast where the consumer needs
it. Object-specific inline twins require ownership or include evidence in
addition to matching bodies. The first monster and boss passes exposed false
associations with other enemies' helpers. Those replacements were reviewed
again under the revised rule.

Four SceneArc and four Hresvelgr diagnostics disappear when the exact
SignedShift twin gives way to the established Math_ShiftTowardZero operation.
The shift arguments keep their original values. The mechanical warning totals
include that change in classification and one folded local in the skeleton
code.

Each source change was gated by stock `tools/check.py`. New and changed
headers were checked through their dependents. The final sweep matched all
310 changed units and all 228 dependents of the 61 changed headers. Compiler
mismatches and ambiguous type or source uses remain in the worker's family logs.
