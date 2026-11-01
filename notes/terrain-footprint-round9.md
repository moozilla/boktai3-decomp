# Terrain_MoveFootprint

The guarded footprint mover reaches 82.2% fuzzy from 20.3%. A corner probe can
change its input position. The caller undoes the footprint offset on that
adjusted position before sampling the surface. A diagonal uses independent
positions for the X-only and Z-only probes. When both axes are blocked, their
movement signs select the boundaries of the current tile.

The reconstructed candidate has the same 2,128-byte extent and 47 register-blind
instruction differences as the earlier draft tested across 432 combinations
of direction, starting position and probe outcomes.
