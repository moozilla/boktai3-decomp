#include "global.h"
struct G { u8 a[0xc]; s32 w; };
extern struct G *gUnk_030053F8; s32 sub_0821ABA8(u32, s32);
s32 sub_082281FC(void) { s32 v = sub_0821ABA8(0x70, gUnk_030053F8->w); if (v < 0) v = 0; else if (v > 9999) v = 9999; gUnk_030053F8->w = v; return v; }
