# ScenePanel_DrawOverlay (08058BD8): holdout

Tags: #matching #phase3

1056 bytes, 8 pins and 5 operand barriers in the landed match.

* `ScenePanel_DrawOverlay.no_pins.c`: the landed draft with every pin and barrier removed
  (`scope.py pins`): 159 ROM / 159 draft. The frame is 0x64 against the ROM's 0x68 (one
  spill slot fewer), and `why all` lists 36 mechanisms, mostly allocation order
  (`AGBCC_PRI=target:+11004` alone gives 136/134) and reload rotation.
* `ScenePanel_DrawOverlay.natural.c`: a fresh rewrite in plain C (for loop with continue,
  direct gBgRecords[3] fields, array indexing): 270 / 262. Not yet worked.
* ROM details worth keeping: the slot counter lives in a stack slot ([sp+0x5c]) and is
  incremented twice, once before the `current == target` test (the continue path) and once
  after the switch; `&entry` (sp+0x18) is held in a spilled pseudo ([sp+0x58]), likely a
  loop-invariant hoist; the out-of-range and both parity arms end in one shared `strh r1, [r0]`
  (jump2 cross-jump).

Untried: matchsearch store; working the natural draft hunk by hunk.
