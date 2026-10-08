#include "global.h"
struct E { u8 p[0x160]; };
struct A { u8 p0[0x18]; u32 w18; struct E e[3]; };
struct E *sub_081FFE60(struct A *a, s32 *out) { struct E *e = a->e; s32 i = 0; u32 m = 1; u32 w = a->w18; do { if ((m << i & w) != 0) { i++; e++; continue; } *out = i; return e; } while (i <= 2); return 0; }
