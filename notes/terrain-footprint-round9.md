# Terrain_MoveFootprint

All 2,176 bytes match. Cardinal movement tests two leading corners. Diagonal
movement tests the leading corner, then the two axes independently. The
caller removes its footprint offset from a corrected probe before sampling
the surface. If both axes are blocked, the motion signs select the edges of
the current cell.

Each diagonal has its own exit that sets the adjustment byte. An X-only
correction shares that exit across directions with the same X sign. Keeping
the exit labels just after the final Z-probe calls lets agbcc share the two
matching instruction tails. Moving those writes to the bottom of the function
changes the branch destinations.

Tags: 0821C8F8, Terrain_MoveFootprint, terrain, matching
