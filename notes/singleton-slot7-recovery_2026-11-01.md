# Slot-seven presentation

Tags: 081F1274 SingletonSlot7_UpdatePresentation

SingletonSlot7_UpdatePresentation matches all 2,208 bytes. The palette restore
and four-bank snapshot share the destination pointer. The restore begins at
background bank 4. The snapshot copies in the opposite direction.

Tags: 081F0734 SingletonSlot7_DispatchPendingCommands

The dispatcher matches all 1,776 bytes. Five paths snapshot four palette
banks, then record the source and destination rows for the transition.
Command 16 retains both argument tests and repeats the dark transition.

Tags: 081F1C2C SingletonSlot7_InitFromScript

SingletonSlot7_InitFromScript matches all 1,032 bytes. Separate indices
fill the white palette and snapshot the four background banks. The
byte-valued hold flag survives the palette bank setup before both drawable
modes are set.

Tags: 081EEECC SingletonSlot7_UpdateShakingDeparture

All 608 bytes match. The actor jitters two display coordinates for the
first 60 frames, then emits a particle on odd frames. The spawn position
uses the low halfword of the foreground sprite's x coordinate before
adding the horizontal jitter.

Loading the jitter sample address through its two GBA halfwords lets agbcc
keep the first table pointer on the stack. In a diagnostic build,
suppressing its early register-equivalence note on an ordinary pointer
gave the same code. The later particle draws use the shared RAM symbol.
