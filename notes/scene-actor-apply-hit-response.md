# SceneActorReaction_ApplyHitResponse

Matched all 2,132 bytes at 0807D2E0 in ordinary C, without register pins.
The three-particle path and the two damage-display paths share the actor
state. The BIOS fill source stays at sp+0x10.

After clearing the motion flags, the callback keeps their address for the
lethal branch. In modes 0 and 1, reading the response kind before the two
status updates lets PRE save the status-field address in r8. Capturing the
control record before testing the control flag places the two zero values in
r6 and r5. The tracking record supplies the base for its byte flag update.

The remaining four bytes exchanged sp+0x6c and sp+0x70 in the initial
particle loop. Materializing the motion test and half-health sign check as
byte predicates gives the compiler the ROM's stack-slot order. The defeat
sparks keep the table-address view from the earlier draft for allocation.
