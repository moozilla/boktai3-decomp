# HUD page input and script configuration foundry pass

Tags: #foundry #matching #scene-hud #input #scripts

Five exact scratch candidates, in assignment order: 08176974 (page input),
08176B00 (page fade-out), 08177C00 (script pointer options), 08177C9C
(ten page visibility flags and enabled-page selection), 081781B4 (timed state
parameters). Candidates remain in `wip/foundry/`; result records are under
`build/foundry/`. No production integration or code-percentage gain is claimed.

08176E20 is a 95.2% fuzzy draft. It tests Start input, the HUD owner/player
kind, action mode, status flags, control mode, global restrictions and history
direction mode. CORE distance is one: agbcc derives the 0x45F offset from
0x418 with an add, whereas the ROM loads a separate literal. Pointer casts
and integer-address spelling did not change the result; the retained source
uses the shared Player fields.

All six candidates use `SceneHudState` from `include/scene_hud_display.h`.
Replaced only unknown bytes with page fade/callback/repeat fields, script
pointer options, page visibility and timed-state parameters. Header checks
preserve all seven matched consumers. The timed-state display meaning and
new names are provisional; the observed behavior is a 300-frame timer,
nonnegative start value and phase zero or six. No register pins or assembly.
