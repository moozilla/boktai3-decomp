#include "global.h"
extern u8 *gUnk_02000710;
void sub_0822E2D4(s32 a) { s32 i = 0; do { u8 *g = gUnk_02000710; s16 *q = (s16 *)(i * 2 + (u32)g + 0x58); if (*q == a) *q = -1; i++; } while (i <= 3); }
