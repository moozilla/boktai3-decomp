#include "global.h"
struct E { u8 p[0x3ac]; };
extern u8 *gUnk_020005EC;
void sub_081FE16C(u8 *, struct E *, s32); void sub_082151E4(u8 *, u32); u32 sub_0802140C(u32);
u32 sub_081FE68C(u8 *a) { struct E *e; s32 i; gUnk_020005EC = a; *(u32 *)(a + 0x18) = 0; e = (struct E *)(a + 0x38); i = 0; do { sub_081FE16C(a, e, i); i++; e++; } while (i <= 3); sub_082151E4(a + 0x1c, 0x3641); *(u32 *)(a + 0xEF4) = sub_0802140C(7); return 0; }
