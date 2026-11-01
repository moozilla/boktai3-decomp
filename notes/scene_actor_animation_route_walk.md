# Scene actor route walk

Tags: #matching #scene-actors 0802F92C

SceneActorAnimation_UpdateRouteWalk matches all 572 bytes. It starts the route,
aims at the first terrain-adjusted point, and moves 24 or 48 units when the
keyframe status permits a step. The route cursor begins at 0x19C and owns the
point pointer at 0x1A4. The previous draft placed the cursor two bytes late
because agbcc rounds the embedded animation structure to word alignment.
