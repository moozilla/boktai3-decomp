# Pending port source repairs

The patch headers name the Thumb site and the value the original caller
inherits. Each patch is scoped to one translation unit. Apply the patches to
the reorganized source after confirming the symbol locations, then run the
per-unit ROM check.

To apply the patches to generated port copies in the current layout:

```sh
build/venv/bin/python port/build_game.py --source-patches tools/autopilot/pending_src_fixes
```

## Reorganized source layout

The worker archive at `build/return-layout-audit/reorganized-return-overlays.zip`
targets main revision `744333be7`. It contains the 85 relocated patches in
`mapped/`, their ROM-address mapping and the verification records. All 76
combined source units passed the per-unit ROM check and wasm32 syntax checks
with `UBFIX` enabled for the latter. The C additions and removals are identical
to those in this directory. The scene stimulus destroy patch at `0822B8E0`
also needed its include context updated.

The port overlay loader accepts the relocated directory once it is combined
with the reorganized source discovery. A cold build against the archived
revision pointed `--source-patches` at the extracted `mapped/` directory
and linked with zero undefined symbols and zero signature mismatches.
The resulting port matched all 13 short recordings and seed 1 at
checkpoints 0022 and 0065. The logs and artifact hashes are under
`build/return-layout-integration/`.

## Other reported direct uses

| Function | ROM or caller evidence |
| --- | --- |
| DirectionalController_SetState | r0 still holds the controller pointer. Both reported uses are calls after labels with discarded results. |
| SceneActor_GetDirectionFlags | The mask and shift restrict the switch value to 0 through 3. Every possible value returns explicitly. |
| ProximityVolume_TranslateBounds | Both reported callers use the filled probe and discard the return in a comma expression. |
| ResultTutorial_MovePlayer | r0 still holds the tutorial pointer. Both reported uses are calls after switch labels. |
| WirelessPeerPanel_SetAvailable | r0 still holds the owner pointer. The reported caller discards it. |
