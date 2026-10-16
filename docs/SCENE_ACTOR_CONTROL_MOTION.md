# Actor control motion phase, 08083550–08083AEC

This contiguous phase extends the actor control context at actor offset 3D0. Names describe observed behavior; the preset's broader gameplay identity remains tentative. The reviewed entries have independent prologues/returns and full literal-pool extents: 83550 (16 bytes), 83560 (628), 837D4 (216), 838AC (480), and 83A8C (96).

The context extends the existing control-phase prefix through storedPosition434. New fields include signed swayStep3E3 and sway3E4, previousGround3EA, three eight-byte ContactPosition vectors at41C/424/42C, and storedPosition434. The shared prefix retains flags3D4, baseOffset3DE, phase3E2, and elapsed/startTick3E6/3E7. The phase dispatcher supplies a valid actor with an initialized control context; 838AC obtains its context before checking availability.

83550 installs the constant preset at08605C28 (existing exact16 bytes, no new credit). 83560 restores the stored position, derives terrain cell and slope-adjusted height, preserves the old vertical difference, clears three vectors, selects animation0, and advances the captured drawable's packed sequence. Its complete nonmatching draft is628 bytes/92.23%; the full628-byte assembly fallback remains authoritative. The draft reuses one initialized CpuSet zero word for the three vector clears and preserves the original packed-command read order.

837D4 consumes context flag2 only when the actor is available and a local player exists. It clears that flag, sets actorEB bit3 and flags2D4 bit8, and enters state0/phase0/behavior3 with cleared counter and animation reset. Its full216 bytes are exact with four initialized register/lifetime hints remaining after reduction from22.

838AC follows changing ground height and adds a signed sine bob. Flag2 uses the saved base offset immediately; flag1 permits an early ground-only placement while the signed vertical offset is at most259. Otherwise it adjusts the vertical offset toward the ground by two units when the difference exceeds16, compensates for changing ground, and updates the drawable Y and phase. Its full480 bytes are exact after reducing15 register/lifetime hints to three scoped initialized register variables. The context getter uses a temporary r5 value rather than constraining the long-lived context pointer; the final offset and ground values use r0/r4. Signed sine samples are bounded halfwords, so the helper's s32 negation is defined.

83A8C clamps the prior signed sway outside ±320, reverses its signed step, then advances by12*step. The resulting one-tick overshoot to±332 is intentional. Its full96 bytes match ordinary C.

Current new exact gain is792 bytes, approximately0.0328 percentage points:96 ordinary and696 register-pinned. The existing16-byte preset helper was reviewed and typed without additional credit. The remaining628-byte restoration draft is uncredited and retains its assembly fallback. No handwritten instruction bodies or explicit r7 bindings were introduced.
