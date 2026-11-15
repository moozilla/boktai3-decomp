# SceneActorMotion_Update (0807A8CC): holdout

Tags: #matching #phase3

1736 bytes with 86 pin and barrier sites. Removing all of them (`scope.py pins`) gives
292 ROM / 274 draft (+43 moved branches); no single removal is free. The draft is a
pin-by-pin reconstruction of the allocation, so the next attempt should start from a fresh
rewrite from the ROM (and likely recovered shared helpers), not from this C.
