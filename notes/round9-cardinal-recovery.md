The guarded 0x0807AF94 callback reaches 79.1% after replacing private pursuit,
terrain and navigation code with the matching Mummy helpers. It tests the two
perpendicular directions in order and starts a two-tick turn before requesting
a route to the center of the last open cell. The helper expansions and temporary
lifetimes still differ across the callback.
