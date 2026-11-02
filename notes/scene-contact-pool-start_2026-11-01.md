ContactActorPool_Start matches all 560 bytes with stock agbcc.

The collider, sequence, sprite and graphics follow the same layout as the adjacent contact swarm. Reading the sprite flags and graphics palette pointer through this layout makes the first loop pass establish the health-collider cursor. The random-table address is assembled from its two halfwords. Capturing the pointer when the first random index is consumed leaves its load for the second loop pass, after the cursor setup.
