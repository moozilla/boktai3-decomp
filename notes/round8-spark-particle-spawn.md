# SparkParticle_Spawn

All 704 bytes at 0806A160 match with stock agbcc. The word that later holds
the random table first captures the incoming emission angle. The incoming
angle word then holds the requested count. Saving the captured direction
before narrowing the count places the direction at SP+0x0c and the table
at SP+0x18. The pool-handle load can use r0 directly, and each random draw
reloads the table from its stack home.
