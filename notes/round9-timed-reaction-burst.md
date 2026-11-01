# Timed reaction burst (08081C98)

Tags: #matching #scene-actors

The guarded callback now uses the same three-part contact burst as the registry
and attachment handlers. Both its single emission and its three-part loop use
the same ten-bit random cursor. The palette branches select the actor's
alternate palette and alternate offset.

The candidate reaches 68.3% on the 1,260-byte extent. Loop register allocation,
the order of stack spills and the control reset still differ.
