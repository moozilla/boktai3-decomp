#include "global.h"
extern u8 *gUnk_02000710; void sub_0822D858(u32);
void sub_0822D918(s32 a) { s32 i; sub_0822D858(a); i = 0; do { u8 *g = gUnk_02000710; s16 *q = (s16 *)(i * 2 + (u32)g + 0x68); if (*q == a) *q = -1; i++; } while (i <= 7); }
