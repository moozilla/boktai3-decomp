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
