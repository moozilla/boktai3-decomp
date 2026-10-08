#include "global.h"
extern u8 *gUnk_020005F4; void sub_081FF948(u8 *, u8 *, s32);
u32 sub_081FFDBC(u8 *a) { u8 *e; s32 i; u16 *q; u32 z; gUnk_020005F4 = a; *(u32 *)(a + 0x18) = 0; e = a + 0x1c; i = 0; do { sub_081FF948(a, e, i); i++; e += 0x160; } while (i <= 2); z = 0; i = 0xf; q = (u16 *)(a + 0x45e); do { *q = z; q--; i--; } while (i >= 0); { u32 w = 0x7FFF; i = 0xf; q = (u16 *)(a + 0x47e); do { *q = w; q--; i--; } while (i >= 0); } return 0; }
