# Shared scene reaction route reset

Tags: #matching #types #phase3 #scene

The resumed scene pass replaces eight private reaction resets and the
status update's reset with one operation on the actor and its declared
route control record. The reset clears the pursuit behavior bits, then
clears the counter, auxiliary halfword and timer. The callers span
contact recovery, defeat, fading, sound, sinking and displacement.
The four writes have the same order in each caller.

All 100 header dependents match. The new behavior caller at 08078EE8
independently matches all 5956 bytes and uses the declared attachment
timer field. Across the resumed pass, 11 changed C units match.

The collider reaction at 0807E9D0 uses plain contact positions for its
three effect arguments and shares the primary-player tracking operation.
The direct table pointer changes address setup and register allocation,
so this burst keeps its existing random table view. The draws in
080800A4, 08080868, 08081290 and 08081C98 use the declared mode,
position or palette fields.

The timed burst at 08081C98 holds the behavior flags pointer across
effect setup. Recomputing that pointer through the shared reset changes
its live range and branch layout. The pending dispatcher at 0807A714
and the two scripted registry transitions at 080704EC and 080706DC
also change their compiled instruction order under the direct or
shared variants. Their exact bodies remain in use.
