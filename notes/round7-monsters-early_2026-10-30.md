# Round 7 early monster matches

Tags: #monsters #ghoul #matching

## Ghoul_TurnTowardRoute (08086400)

Exact 1,580 bytes in ordinary C using the shared turn operations. Initializing the
body, route, and group owners in that order preserves the entry address setup.
Animation nine, frames one and three, and the 0x20 direction step are Ghoul-specific.

Validation: `tools/check.py src/fn/sub_08086400.c --target-size 1580 --json` reports
zero differing bytes.
