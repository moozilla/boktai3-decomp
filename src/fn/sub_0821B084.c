#include "global.h"
#define GameState_SelectRamBase sub_0821B084
// Provisional name: GameState_SelectRamBase
// MATCHING_CONCESSION: register-pinning
/* Select timer-jittered storage for the game-state regions. */
extern u8 gUnk_0203C400[];
extern u8 *gUnk_02000710,*gUnk_0200070C,*gUnk_02000708,*gUnk_02000704,*gUnk_02000700;
void GameState_SelectRamBase(void) {
 u32 timer=*(vu16*)0x0400010C;
 register u8 *base asm("r1")=gUnk_0203C400;
 base+=(timer&127)*8;
 gUnk_02000710=base;gUnk_0200070C=base+0xb00;gUnk_02000708=base+0x1600;gUnk_02000704=base+0x1a00;gUnk_02000700=base+0x1e00;
}
