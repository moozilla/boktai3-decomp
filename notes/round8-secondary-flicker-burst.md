# FlickerParticle_StartSecondaryBurst

Tags: #effects #round8 #matching

All 288 bytes at 08235174 match with stock agbcc after removing four register
pins and two compiler barriers. The particle cursor and independent byte offset
advance together. Initializing the loop count before capturing the RNG mask
and sine table preserves the ROM's ordering and avoids caching the two particle
field addresses across iterations.

The second draw supplies the radius. Its word-sized result gives way to a
halfword phase mask and a signed halfword cosine amplitude. The shared sample
read and the typed phase capture put the cosine mask in r0 at 082351E8.
