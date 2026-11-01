# Contact actors

Tags: 08042984 #contact-actors #matching

`ContactActor_InitializeSlot` matches all 384 bytes. The resource collider's
attack flags are 0x00208000. The halfword initializer keeps one zero across
the state byte, frame counter and direction stores. The sprite position
address is evaluated before the BIOS fill's stack zero.

Tags: 08044AD4 #contact-swarm #matching

`ContactSwarm_SpawnPairedAdvanceCount` matches all 428 bytes. One table
pointer serves the random samples and then the alternating headings.
Computing the slot stride before the loop keeps the incremented slot index
live until the count comparison and fixes the draft's uninitialized stride.
