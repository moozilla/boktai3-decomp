#include "global.h"
void sub_082151E4(void *, u32); void sub_082144E4(void *, void *, u32); extern u8 *gUnk_020000AC;
u32 sub_08233098(u8 *s) { u8 *p; s32 i; gUnk_020000AC = s; p = s + 0x18; i = 0xf; do { u8 *q = p + 0x30; sub_082151E4(q, 0xda6d); sub_082144E4(p + 4, q, 0); i--; p += 0x4c; } while (i >= 0); return 0; }
