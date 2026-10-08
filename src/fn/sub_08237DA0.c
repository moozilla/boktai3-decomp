#include "global.h"
void sub_082151E4(void *, u32); void sub_082144A4(void *, void *, u32); void sub_08215284(void *, u32);
void sub_08237DA0(u8 *s) { u8 *p = s + 0x48; u8 *q; sub_082151E4(p, 0x210e); q = s + 0x1c; sub_082144A4(q, p, 1); *(u16 *)(q + 0x10) = 0; sub_08215284(p, 0x4d); }
