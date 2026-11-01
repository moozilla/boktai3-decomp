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

Tags: 080461D0 #contact-sequence #nonmatching

The guarded death callback wraps all six random draws at 1024, including
the three after the first particle spawn. Its old byte index wrapped at
256 there. The remaining differences are the low-register copy of the
horizontal mask and the register allocation of the first vector addition.

Tags: 08042F44 #contact-actors #nonmatching

The roaming draft now matches the scale calculation in the first turn
sample. Keeping the 256-unit radius in a local makes agbcc shift the
cosine component and multiply the sine component, as in the ROM.
The target-vector address order and allocations across the two steering
branches still differ.
