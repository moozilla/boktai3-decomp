#include "global.h"
extern u8 gUnk_08E60F80[];
u8 *sub_0822E288(u32 x) { if (x > 0x2f) return 0; return gUnk_08E60F80 + x * 16; }
