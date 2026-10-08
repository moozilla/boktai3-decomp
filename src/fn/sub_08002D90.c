#include "global.h"

struct S { u8 f0[0x18]; u16 fl; u8 pa[2]; u8 f1c; u8 pb[3]; u32 n; u8 pc[0x28c-0x24]; u8 b[0x200]; };

u8 *sub_08002AC0(struct S *p, u32 a, u32 b);
void sub_08002AE0(void *, u8 *, u8 *, u32, u32, u32);

void sub_08002D90(struct S *s, u8 *q) {
    u8 *r;
    u8 *d;
    if (s->fl & 1) {
        r = (u8 *)s + 0xA8C;
    } else if (s->fl & 8) {
        r = (u8 *)s + (s->n * 0x200 + 0x28C);
    } else {
        r = sub_08002AC0(s, 0, s->n);
    }
    d = (u8 *)s + 0xC8C;
    sub_08002AE0(d, r, q + 0xC, q[1], 0, 0xD0);
    *(u8 **)((u8 *)s + 0xE90) = d;
    s->f1c = 1;
}
