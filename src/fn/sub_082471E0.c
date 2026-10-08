#include "global.h"
extern u8 *gUnk_03006A60[];
void sub_082471E0(u8 i) { if (i <= 3) { u8 *p = gUnk_03006A60[i]; if (*(u16 *)p == 0x8024) p[2] = 1; } }
