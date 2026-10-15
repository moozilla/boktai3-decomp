# ScenePanel subject final state and driver

Reviewed range: **0805D6C4–0805D92C**, five functions, 616 bytes. Three new functions add **448 exact bytes (+0.0185 percentage points)**, all ordinary C without register pins or empty asm. Existing cleanup and collider initialization account for168 bytes and receive typed contracts. D92C begins the longer initializer and is excluded.

`scene_panel_subject_tail.h` supplies tail API declarations; the consolidated layout is in `scene_panel_subject.h`. Drawable18 contains position34; graphics44, home60, collider68, sequenceBC and resourceCC agree with the adjacent subject initialization. State/callback fields remain D3/23C. Additional observed fields are timerDE, contactCooldownE4, requestedE6, requestLevelE8, holdEC, contactKindEF, paletteBankF2, basePaletteBankF4, countdownF6, nextThresholdF8, byteTimerFD and effect-group storage104. These descriptive meanings are tentative.

| Address | Name | Bytes | Contract |
|---|---|---:|---|
| 0805D6C4 | ScenePanelSubject_UpdateTimedEffect | 172 | Decrement byteTimer if nonzero. While countdown is nonzero, use basePaletteBank, decrement countdown, and invoke082314C4 when it falls below nextThreshold, then subtract250 from that threshold. At zero countdown, invoke08231568 on the first frame, increment the frame counter, and use basePaletteBank-1 through frame15. Afterward restore home position, choose palette bank24E and state4, then clear collider halfwordA6 and the state frame. |
| 0805D770 | ScenePanelSubject_UpdatePaletteBank | 144 | A nonzero requested field raises requestLevel toward12 and is consumed; without a request the level falls toward0. Select palette bank250 at level12,24F at other nonzero levels. At level0, consume contactKind for palette bank192 flashes, otherwise use paletteBankF2. |
| 0805D800 | ScenePanelSubject_Update | 132 | For any nonzero state, run player exclusion. Decrement nonzero timerDE and contactCooldownE4, call the current state callback, then select palette banks. A nonzero holdEC decrements only when global mode is nonzero and the post-callback state is4, clearing collider halfwordA6; otherwise clear hold. Return0. |
| 0805D884 | ScenePanelSubject_Destroy | 36 | Remove the drawable, collider and embedded effect group; return0. |
| 0805D8A8 | ScenePanelSubject_InitCollider | 132 | Configure collider flags4001, argument4=10, size(100,200,100) and offset(0,200,0). Apply the caller's additional setting, bind position to home60, and install the subject contact callback with this owner. |

State7 is selected by callback table08605AF8. Effect helpers remain numeric because their broader lifecycle semantics are not yet established. The threshold subtraction and frame increments preserve u16 wrapping. Palette requestLevel is only capped while incrementing values0–11; existing out-of-range nonzero values still select24F, rather than being normalized.

The driver reads state again after the callback and global mode at the hold check. It assumes a valid global panel when hold is nonzero, matching the original. `contactKind` serves both the contact helper's byteEF write and a remaining flash count in the palette selector; it may be renamed as the shared interpretation develops. Packed collider vectors preserve the original word writes and unspecified padding bits.

Integration confirms that 08215284 selects a 32-byte bank of sixteen RGB15 colors.
The worker baseline name SelectTiles was superseded by SelectPaletteBank on main;
F2/F4 and constants 24C–250 refer to palette banks. State-entry FB/FC remain songs.
