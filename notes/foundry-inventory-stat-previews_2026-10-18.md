# Inventory stat-preview foundry pass

Tags: #foundry #matching #scene-hud #inventory

Scratch candidates only; no production integration or code-percentage gain.
Names are tentative. All candidates remain in `wip/foundry/`, with machine
records under `build/foundry/sub_XXXXXXXX/result.txt`.

- 081744AC: exact, four checks; equipment description script and four string
  arguments. Uses SceneValueEquipment and its definition table at 08E6092C;
  the table identity is confirmed by the matched accessor 08227F5C.
- 081748B8: exact, one check; slot coordinates for a four-column grid and
  special slots 32–36 and 39.
- 08174BBC: fuzzy 46.2%, eight checks; positions three rows of amount glyphs
  starting at HUD motionWidgets[2]. CORE distance five, caused by two hoisted
  constants and their setup; register allocation remains different.
- 08174C2C: fuzzy 81.5%, seven checks, CORE zero; caps two stat sums at 120,
  scales by ten or five plus 100, and updates two labels from the third
  comparison offset. Uses Player currentStatA/currentStatB.
- 08174D20: fuzzy 24.7%, four checks, CORE 35; three marker rows at 3400,
  3520 and 3640 use base minus 100, then comparison offsets, then deltas.
  Retained the lower-CORE pointer draft; the simpler typed-array draft scored
  28.3% but had CORE 92. Pointer induction merging and constant hoisting remain.
- 08174F48: exact, two checks; key bit 4 selects alternate preview rendering
  and sets HUD byte A58; the common tail sets the hide flag on two indicators.

Shared types: replaced padding in scene_item_values.h with description table
fields and in scene_hud_display.h with A58 modifier state, indicators at 3340,
three-by-three markers at 3400, and signed deltas at 48B0. No existing offset
moved. Both hdrcheck runs preserved all seven matched consumers of each header.
No pins, handwritten instructions, commits or combined builds were used.
