#include "global.h"
struct S { u8 p[0x18]; s16 id; }; extern struct S *gUnk_020004B4; void sub_0822B358(u32);
static inline s32 gid(void) { if (gUnk_020004B4 == 0) return -1; return gUnk_020004B4->id; }
void sub_0823849C(s32 a, u32 b) { if (a == gid()) sub_0822B358(b); }
