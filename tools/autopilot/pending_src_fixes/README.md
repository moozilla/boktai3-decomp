# Pending port source repairs

The patch headers name the Thumb site and the value the original caller
inherits. Each patch is scoped to one translation unit. Apply the patches to
the reorganized source after confirming the symbol locations, then run the
per-unit ROM check.

To apply the patches to generated port copies in the current layout:

```sh
build/venv/bin/python port/build_game.py --source-patches tools/autopilot/pending_src_fixes
```

## Other reported direct uses

| Function | ROM or caller evidence |
| --- | --- |
| DirectionalController_SetState | r0 still holds the controller pointer. Both reported uses are calls after labels with discarded results. |
| SceneActor_GetDirectionFlags | The mask and shift restrict the switch value to 0 through 3. Every possible value returns explicitly. |
| ProximityVolume_TranslateBounds | Both reported callers use the filled probe and discard the return in a comma expression. |
| ResultTutorial_MovePlayer | r0 still holds the tutorial pointer. Both reported uses are calls after switch labels. |
| WirelessPeerPanel_SetAvailable | r0 still holds the owner pointer. The reported caller discards it. |
