#include "global.h"
struct S { u8 p[0x18]; s16 id; }; extern struct S *gUnk_020004B4; void sub_08226508(u32);
static inline s32 gid(void) { if (gUnk_020004B4 == 0) return -1; return gUnk_020004B4->id; }
void sub_0823B6A4(u8 *s) { if (*(s32 *)(s + 0x18) == gid()) sub_08226508(8); *(u16 *)(s + 0x4b6) = 0; }
