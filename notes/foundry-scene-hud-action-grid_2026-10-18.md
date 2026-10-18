# Scene HUD action grid foundry candidates

Tags: #foundry #matching #scene-hud #input

Prepared candidates in assignment order, retained under `wip/foundry/`; no
production promotion, commits, or full-ROM build. Production progress gain is
0 percentage points; five scratch candidates are exact, one is fuzzy.

| Function | Tentative name | Foundry result | Checks |
| --- | --- | --- | --- |
| 08171F7C | SceneHud_InitializeActionPanel | fuzzy 77.2% | 4 |
| 081723D8 | SceneHud_ShowSelectionDescription | MATCH | 2 |
| 081724D4 | SceneHud_RefreshEquippedActionIcons | MATCH | 3 |
| 08172850 | SceneHud_ConfigureActionGrid | MATCH | 4 |
| 08172950 | SceneHud_LayoutActionGrid | MATCH | 4 |
| 08172BD0 | SceneHud_UpdateActionGridInput | MATCH | 2 |

08171F7C's call/store structure is recovered, but its widget-address generation
and register choices differ. Several hypotheses produced the same score, so
work stopped early under the brief. Its trailing bytes include independent
prologues at 08172094 onward; candidate-length scoring also exposes these when
the candidate is longer than the primary body. It is a real initializer, not
a split-only tail.

081723D8 maps cursor values 0..7 through 0822EA70, and values 8..19 through
GameState_GetUnlockEntry. Empty selections hide the description animation and
clear the text. Sequential string-index additions avoid unwanted reassociation.

081724D4 reads signed slot values from game-state offsets 0x50 and 0x54,
then indexes s16 tables at 0x78 and 0x90. These overlap existing inventory/slot
arrays, so their established layouts were retained. Two icon widgets at HUD
0x42A4 and 0x4304 share a resource at 0x164; a palette at 0x4364 is copied
from the animation palette table. Scoped widget pointers preserve the original
flag-access bases and avoid address reuse.

08172850 establishes a four-column, nine-row byte grid at input offset 0xC,
initializes cells with 0xFF, fills the action/unlock ranges by mode, and maps
flags 1/0x20/0x40/0x80 to auxiliary values 0x20/0x26/0x27/0x28. An inline
u8 result helper recreates conditional materialization; explicit next-column
locals recreate loop ordering.

08172950 places eight action icons in two columns at (80,32), and twelve
unlock icons in three columns at (136,32), each with 24-pixel spacing. A
SceneVec setter plus a ContactPosition copy reproduces its stack operations.
081727AC writes the same cursor position layout (confirmed directly from asm).
08172BD0 processes movement through 0816B59C, A confirmation, and B cancellation;
HUD byte 0x4388 distinguishes which confirmation routine to invoke.

All layout additions are in `include/scene_hud_display.h`, replacing unknown
padding without moving existing fields. The category widget extent is now 30,
ending at 0x1F60. Each header edit passed foundry hdrcheck: all seven existing
matched consumers remained exact. Other pre-existing workspace changes were
left intact. Per-function result files are under `build/foundry/`.
