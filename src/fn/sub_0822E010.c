#include "global.h"
extern u8 *gUnk_02000710;
void sub_0822E010(void) { s32 i = 0; do { *(s16 *)(gUnk_02000710 + i * 2 + 0x68) = -1; i++; } while (i <= 7); *(u16 *)(gUnk_02000710 + 0x50) = 8; }
