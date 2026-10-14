# Modifier selection and stat previews (tentative)

This contiguous pass covers **0803F804–080408A4**, immediately after the arc
pool and ground projection. It is a selection/stat-preview menu; its specific
in-game identity remains tentative. Static field consumers establish the names.

Matching rises from **22.529% to 22.627%**, **+0.098 percentage points**
(+2,376 exact bytes, 13 functions). Session gain from Claude's 22.309% baseline:
**+0.318 points**, 35 new exact functions, +7,676 bytes. Without register pinning:
18.527%; Register-pinned: 4.100%, included above. This batch has 1,472 bytes
without pins and 904 with pins. The collector draft adds no exact credit.

| Address | New exact function | Reviewed bytes |
|---|---|---:|
| 0803FA5C | Modifier_ApplyToStat | 108 |
| 0803FC8C | Modifier_LoadFourthSource | 56 |
| 0803FCC4 | ModifierMenu_SelectPendingSource | 160 |
| 0803FDBC | ModifierMenu_ReconcileSelection | 172 |
| 0803FEBC | ModifierMenu_CommitSelection | 24 |
| 0803FED4 | ModifierMenu_ToggleSelection | 76 |
| 0803FF40 | ModifierMenu_CloseScript | 64 |
| 0803FFC8 | ModifierMenu_ApplyPendingSource | 208 |
| 080401E8 | ModifierMenu_DrawFirstSourceStats | 232 |
| 080402D0 | ModifierMenu_DrawSecondSourceStats | 540 |
| 080404EC | ModifierMenu_DrawThirdSourceStats | 292 |
| 08040610 | ModifierMenu_DrawFourthSourceStats | 372 |
| 0804085C | ModifierMenu_DrawBackground | 72 |

## Recovered contracts

`ModifierMenu` describes the known prefix, not the full task allocation. Its
signed state/mode fields are at +0x18/+0x1c, category/index at +0x20/+0x21,
background resources at +0x34/+0x38, and current/pending snapshots at +0x68/
+0x90. Each snapshot is 0x28 bytes: signed stat halfwords, selected count and
capacity at +0xa/+0xc, a 32-bit selection mask at +0x18, four source IDs at
+0x1c and primary/secondary selected IDs at +0x24/+0x26. The first parameter
of the stat modifier routines is also this snapshot type, not an actor object.

Mode zero chooses one of four source slots and loads its stats from the item
lookup helpers 0811E48C/0811E5D0/0811E3F8/0811E534. Mode one forms a source ID
as category*6+index and toggles its bit, refusing additions at capacity. Mode
two selects the secondary ID. Availability helper 0811DD94 gates application.
Reconciliation keeps the first capacity-count set bits and writes up to four
selected source IDs to GameState +0x7a8. Existing commits write source choices
back to +0x7a0/+0x7a4/+0x7a2/+0x7a6; secondary selection is at +0x81a.

The global source-ID storage remains an eight-byte view named
`modifierIdsBytes`, accessed as signed halfwords. Replacing it with an s16
array changed agbcc's store lowering and lost exactness; the byte view preserves
the observed reloads/stores. The named layout and matching C are retained.

`ModifierRecord` is eight bytes: signed order, kind, unsigned amount, one extra
byte and unknown tail bytes. The stat application function initializes eight
record order fields to -1, calls the collector, applies matching kinds and
caps the result at 999. Kind 0, 7-10 and 12 use value*amount/10 when amount is
nonzero; the other kinds add amount. Names preserve numeric kinds pending a
better game-stat mapping. The first-source positive-kind wrapper also returns
zero for a nonpositive base value.

State one renders valid pending comparisons, or blank placeholders for
unavailable choices. States zero/two render current values. The second-source
renderer also compares capacity; the fourth renderer adds paired unit glyphs.
The background helper displays its active variant in state one and resets
background zero in state two. The reset and second-source lookup calls preserve
observed unused arguments; their existing C declarations were corrected and
exactly rechecked.

## Retained collector and unsuccessful experiments

0803F95C (`Modifier_CollectSources`) is a **256-byte reviewed** guarded draft,
**88.1% fuzzy**, with **32 differing bytes and no size delta**. It collects
records for current selected IDs, appends the pending selected ID if absent
and includeSelected==1, then bubble-sorts four records by nonnegative order.
The first buffer has eight initialized sentinel slots; sorting makes four
passes over three adjacent pairs and ignores negative record orders.

The sort and collection structure now match. The remaining differences are
the initial argument spill, temporary pointer/load order in the selected-ID
scan, and register choices around appending the pending record. Broad pointer
pins and direct index-register constraints produced extra copies/reloads and
worse scores. A 45-second bounded permuter attempt failed with its existing
`nodes should only appear once in AST` assertion (seed 49471845922707170).
No output was accepted. The simpler best full-size candidate is retained;
more pointer-pinning without a new lifetime hypothesis is not useful.

## Validation and continuation

All new exact functions passed explicit full-extent byte checks. Pins/barriers
were minimized with depin.py; most additions need none. Existing typed mask
helpers, stat wrappers and corrected hidden-argument declarations also match.
The combined build printed `build/boktai3.gba: OK`
(`build/modifier-menu-combined.log`) before integration. Main-only progress Actions remain in force. The former gap at
0803F8F4 was already registered and matched by Claude; naming it earns no new
credit, and its local alias remains because it is a hidden assembly boundary.

Next contiguous subsection starts at **080408A4**: graphics-resource setup,
menu input/update and remaining task lifecycle routines. Initial disassembly
shows 32-byte resources at +0x13c/+0x15c and eighteen 0x60-byte display objects
starting at +0x17c. Reuse these observations, but confirm the full layout before
changing the shared prefix. Latest usage observation: **92% remaining**;
continue until 25% exact C or 80% remaining. Claude's original checkout edit
remains intact.
