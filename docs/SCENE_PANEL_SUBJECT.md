# ScenePanel constructor and subject helpers

Reviewed range: **0805CB38–0805CD64**, six functions, 556 bytes. Five newly recovered functions add **552 exact bytes (+0.0229 percentage points)**: 440 ordinary C and 112 with one minimized register pin. The hidden CD60 four-byte no-op was existing C and gains no credit. Every full reviewed extent matches. CDE0 is a literal pool inside CD64, so it is not a valid function boundary; CD64 and later state handlers belong to the next subsection.

`scene_panel_subject.h` provides a tentative partial subject view. Observed fields include position34, home60, stateD3, stateFrameD4, contactProgressD6, contactLimitD8, contactCooldownE4, requestedE6 (from the existing rig view), contactKindEF, changedF0, positionResetF1, and callback23C. Broader meanings of contact progress and requested remain tentative. The callback table08605AF8 has eight entries; this setter assumes a valid state index.

| Address | Name | Bytes | Contract |
|---|---|---:|---|
| 0805CB38 | ScenePanel_Create | 80 | Return the existing singleton when present. Otherwise allocate17E4 bytes in task list9, install driver/destructor, narrow actor ID to16 bits, and initialize. A negative init result kills the task and returns null; allocation failure also returns null. |
| 0805CB88 | ScenePanelSubject_SetState | 76 | Narrow state to a byte, reset its halfword frame counter and choose its callback. Entering state3 plays songFB, entering state5 playsFC. |
| 0805CBD4 | ScenePanelSubject_ResetPosition | 112 | Copy all eight home-position bytes, set positionResetF1, then independently jitter X and Z by `sample % 10 - 5`. Consume two random table samples, preserving Y/padding. One local R1 pin remains after minimization. |
| 0805CC44 | ScenePanelSubject_KeepPlayerOutside | 172 | If absolute player/home X and Z deltas are each at most127 and their squared distance is at most3FFF, project the player's X/Z along the returned angle using `(sine * 65) >> 11`, relative to home. Keep Y unchanged. |
| 0805CCF0 | ScenePanelSubject_ApplyContact | 112 | Require a low-seven-bit reaction flag, zero contact cooldown, and state2. Add the colliders' halfword3C difference to progress, substituting1 for a negative difference; a zero difference stays zero. Narrow progress to16 bits before comparing against its limit. Reaching the limit clamps progress and selects state3; otherwise set cooldown12 and play song127. Set contactKind4 in either accepted case. |
| 0805CD60 | ScenePanelSubject_Idle | 4 | No-op state0 callback. |

The contact callback keeps its third argument as opaque owner data until the reaction test succeeds, then views it as a subject. Its first two arguments are collider views, not subject-position pointers. Existing rig callers declare the state setter with `SceneRigSubject *`; the setter casts into the expanded view internally, preserving the established u32 state argument ABI. Integration may unify those layout-compatible partial views.

The constructor's actual lifecycle callbacks return s32; their forward declarations use ScenePanelLifecycleView to agree with the adjoining cohort. Task_SetCallbacks' older generic void callback declaration requires an explicit function-pointer cast at that boundary; no callback result is used here. The constructor treats any nonnegative initializer result as success.

Integration confirms that 08215284 selects a 32-byte bank of sixteen RGB15 colors.
The worker baseline name SelectTiles was superseded by SelectPaletteBank on main;
F2/F4 and constants 24C–250 refer to palette banks. State-entry FB/FC remain songs.
