#include "global.h"
extern void *gUnk_0200005C; void sub_08217EAC(void *); void sub_08231480(void *, void *);
u32 sub_0823233C(u8 *s) { u8 *p = s + 0x1c; s32 i = 3; do { sub_08217EAC(p); p += 0x48; i--; } while (i >= 0); if (gUnk_0200005C) sub_08231480(gUnk_0200005C, s); return 0; }
