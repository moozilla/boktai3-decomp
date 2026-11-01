# Actor_UpdateRegionChildren

All 248 bytes match. The first pass scans each child's region list, then sets
its hidden flag. Keeping the sprite pointer local to that pass preserves the
separate address used by the callback pass. Only active, visible children
decrement their timer and receive the update callback.
