#include "global.h"
u32 sub_0822E040(u32);
u32 sub_0822E3F0(void) { u32 n = 0; s32 i = 0; do { if (i == 0x21 || i == 0x1a || i == 0x23 || i == 0x24) { } else if (sub_0822E040(i)) n++; i++; } while (i <= 0x2f); return n; }
