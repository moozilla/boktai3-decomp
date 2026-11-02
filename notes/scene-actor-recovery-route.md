# SceneActorRoute_TryRecoveryDetour

Matched all 4,488 bytes at 0807AF94 in ordinary C. A navigation probe can
send a Mummy or another contact-route actor through this routine. It tries
the two directions perpendicular to the current facing, follows the open cells
in the chosen direction, and sets the recovery gate on success. A tracked
target at a different height takes the direct-arrival fallback.

Two successive uses of the same machine word decide the tail. The selected
cell and the address of the gate it sets both occupy r6. Giving the final
gate load its own assignment keeps it out of the compiler's reload rotation.
The row quotient and the subsequent route test both occupy r5. Keeping their
results in one local leaves r6 available across construction of the cell
center. The turn loop samples through a 16-bit index and converts the heading
to eight bits before comparing it with the current facing.
