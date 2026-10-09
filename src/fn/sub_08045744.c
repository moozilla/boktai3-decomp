#include "global.h"
struct E { u8 p[0x13c]; };
struct A { u8 p0[0x18]; u32 w18; struct E e[16]; };
struct E *sub_08045744(struct A *a, s32 *out) { struct E *e = a->e; s32 i = 0; u32 m = 1; u32 w = a->w18; do { if ((m << i & w) != 0) { i++; e++; continue; } *out = i; return e; } while (i <= 15); return 0; }
