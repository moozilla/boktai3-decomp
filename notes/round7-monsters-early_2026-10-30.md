# Round 7 early monster matches

Tags: #monsters #ghoul #matching

## Ghoul_TurnTowardRoute (08086400)

Exact 1,580 bytes in ordinary C using the shared turn operations. Initializing the
body, route, and group owners in that order preserves the entry address setup.
Animation nine, frames one and three, and the 0x20 direction step are Ghoul-specific.

Validation: `tools/check.py src/fn/sub_08086400.c --target-size 1580 --json` reports
zero differing bytes.

## Ghoul_ResumeDelayedChase (080898FC)

Exact 2,344 bytes using canonical chase, saved-response, and state-entry operations.
The group owner is captured at entry; the zero result of the blocking query supplies
the later state reset. Stock `tools/check.py` reports zero differing bytes.

## Ghoul disc route recovery (0808A664)

Exact 524 bytes. Canonical state-entry, controller, delayed-response, and route
operations replace raw actor offsets. The motion-flags operation accepts its actor
owner so its mask is prepared before the flag address. The context view exposes
the disc targets and recovery delay. Stock check reports zero differing bytes.
