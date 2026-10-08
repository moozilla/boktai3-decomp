#include "global.h"
extern u8 *gUnk_02000710;
s32 sub_08210A8C(void) { s32 n = 0; s32 i = 0; s32 v = *(s32 *)(gUnk_02000710 + 0x728); do { if ((v >> i) & 1) n++; i++; } while (i <= 0x1a); return n; }
