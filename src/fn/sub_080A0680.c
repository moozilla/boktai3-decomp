#include "global.h"
struct Q { u32 words[0xf6]; };
struct S { u8 pad[0x2D0]; u32 field; u8 rest[0x3D0-0x2D4]; u32 q; };
void sub_080A0680(struct S *s) {
    struct Q *q = (struct Q *)s->q;
    u32 m = 0x100000;
    u32 *p = &s->field;
    u32 *p2;
    u32 m2;
    *p = *p | m;
    p2 = &q->words[0xf5];
    m2 = ~1;
    *p2 = *p2 & m2;
    ((u16 *)s)[0xc7] &= ~4;
}
