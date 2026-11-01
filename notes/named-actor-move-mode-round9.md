# NamedActor_SetMoveModeFromScript

All 136 bytes match. The two flags are updated through the shared halfword
helpers. In mode four, the separate clear and set collapse to one store, while
the implicit halfword zero from the earlier state assignment survives in r6.
The script value is compared at full width, so mode 258 does not enable bit 1.

Tags: #matching #round9b #named-object
