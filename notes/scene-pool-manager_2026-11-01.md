ScenePool_UpdateManager matches all 160 bytes with stock agbcc.

The mask and count start at zero. A 32-bit scan word holds the byte offset of the region array in its low half and the starting player index in its high half. Both halves start clear. Replacing the low half with the field offset after the resets keeps r1 live while the region pointer is formed in r3, then allows the counter zero to be rematerialized in r4. The scan-word representation controls allocation; it is provisional.
