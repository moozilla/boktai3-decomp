#include "global.h"
// MATCHING_CONCESSION: register-pinning
/* Select timer-jittered storage for the game-state regions. */
extern u8 gUnk_0203C400[];
extern u8 *gUnk_02000710,*gUnk_0200070C,*gUnk_02000708,*gUnk_02000704,*gUnk_02000700;
void sub_0821B084(void) {
 register u32 timer asm("r2")=*(vu16*)0x0400010C;
 register u8 *base asm("r1")=gUnk_0203C400;
 base+=(timer&127)*8;
 gUnk_02000710=base;gUnk_0200070C=base+0xb00;gUnk_02000708=base+0x1600;gUnk_02000704=base+0x1a00;gUnk_02000700=base+0x1e00;
}
