# Scene inventory panel: values, descriptions and grids

Static evidence covers `0805F7E0–0806055C`, alongside the input-panel
initializer and comparison helpers in [SCENE_INPUT_PANEL.md](SCENE_INPUT_PANEL.md).
Names describe observed inventory behavior; a particular shop screen and runtime
coverage have not been established.

## Layout and ownership

`SceneInventoryPanel` embeds the input display prefix through `0x14E8`, then five
groups of three comparison widgets with stride `0x120`. Text pointers appear at
`1A8C` (equipment), `1A94` (items), and `1A98` (consumables). Page is at `1AB0`,
the signed catalog view begins at `1AB3`, and selection is at `1BE7`.
The catalog is a phase workspace: it overlaps the transition pair at
`1AF0–1BE4`. Separate local views express the two uses; these are not independent
simultaneously owned objects.

The first sixteen selections address owned slots. The next eight select the
current catalog page. Grid rendering uses four columns, two catalog rows, and
four owned rows. Keyframe palettes `373` and `374` distinguish the observed
availability/value conditions. These constants are palette-bank selections.

## Values

`SceneItemValue_CalculateEquipment` (`5F7E0`, 140 bytes) reads a 16-byte equipment
record: kind at0, condition at2, modifiers at4/8, and grade atC. Kind `FF` or a
zero definition value returns zero. The percentage is20 for condition0,50 for
1–250,80 for251–500,90 for501–750, and100 for751–1000. Values above1000 retain
the default80. Either modifier adds30; grade1 adds10 and grade2 adds20. The
definition's unsigned halfword at6 is multiplied by this percentage, divided
by100 with signed division, and clamped to at least1. The sale handler at62634
adds these calculated values to signed currency at7B0 and removes the selected
owned object, establishing their use as sale prices. The purchase handler at62414
subtracts the definition values instead.

`SceneItemValue_CalculateItem` (`5F87C`, 92 bytes) combines the owned item ID and
slot state. State394 uses50 percent. Otherwise the percentage is
`50 - (state - 395) * 10`, clamped to20–50. The 16-byte item definition holds
its full-word base value atC. The result is divided by100 and clamped to1–9999.
`SceneItemValue_CalculateConsumable` (`5F8E8`, 24 bytes) takes half the signed
halfword at8 in the consumable definition, with no corresponding clamp.

## Description panes

`SceneInventory_ShowItemDescription` (`5FA6C`, 388 bytes) obtains an owned item
and slot-state frame, or a catalog item and frame394/395 selected by the
definition field at4. Missing selections use item-1/frame393. A missing item
hides the banner and clears text. Otherwise it shows the banner, sets the
text window to `(2,16,28,2)`, and parses the stored string reference. The text
offset depends on `GameState.form` at5A8: form6 with item13 uses234; form0/6
uses55+item; other forms use110+item. It refreshes both value and attribute
comparison groups.

`SceneInventory_ShowConsumableDescription` (`5FBF0`, 304 bytes) reads owned IDs
from the signed table at `GameState+160`, or the catalog. It uses frame394,
the same text window, two counters set to999, and string offset48+item, then
updates both comparison groups.

## Grid rendering

`SceneInventory_PopulateEquipmentGrid` (`5FD20`, 536 bytes) uses heading65 and
kind+240 keyframes. Owned kindFF hides a widget; an equipment query selects
palette373 or374. Catalog entries compare signed currency at `GameState+7B0`
against the definition's unsigned halfword at6. Eight packed halfwords at
`GameState+68` position digit widgets at `((packed & 3)*24+112,
(signedPacked >> 2)*24+32)`; negative entries hide them.

`SceneInventory_PopulateItemGrid` (`5FF38`, 420 bytes) uses heading63 and the
item-keyframe lookup. Owned entries use palette373. Catalog entries compare
signed currency against the full-word definition value using the observed
signed comparison. All eight auxiliary digit widgets are hidden.

`SceneInventory_PopulateConsumableGrid` (`600DC`, 616 bytes) uses heading66 and
ID+289 keyframes. Owned negative IDs hide their widgets, but the subsequent
palette path still reads the definition record, including the preceding record
for ID-1. C preserves the observed address calculation with unsigned integer
arithmetic rather than indexing an array at-1. A zero signed definition value
or a nonzero slot query selects374; otherwise373. Catalog entries compare
currency against the signed halfword value. Four packed halfwords at
`GameState+58` position the first four digit widgets; the remaining four hide.

The value refresh functions at `60344`, `603EC`, and `60488` select owned or
catalog values and use the shared grid-position/four-digit helpers. Equipment
and consumable refresh hide nonpositive values; item refresh permits zero.
`60528` searches equipped slots. The action/transition view remains separate
from the catalog view because its storage overlaps.

## Evidence and remaining gap

All listed functions were checked over their complete reviewed extents.
Equipment values, both descriptions, and all three grids retain minimized
register-allocation concessions; item/consumable values and the three refresh
functions are ordinary C. The comparison initializer is documented separately.

`5F904` (360 bytes) remains assembly. Its catalog path initializes only condition
at2 in a temporary equipment record, then calls a helper that also reads kind
at0. The reviewed callers and preceding descriptor copy do not establish that
byte. Do not introduce an uninitialized C read to obtain an exact match; this
needs an explicit representation or additional evidence. The earlier E484 and
E5E4 holes remain guarded drafts and receive no exact credit.
