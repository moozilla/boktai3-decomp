#include "global.h"
struct R { u8 a[8]; u32 fl; u8 b[0x60 - 12]; };
struct P { u8 f0[0x541C]; u8 hdr[0x20]; struct R rows[16]; u8 pad[0x71DC - 0x543C - 0x600]; u8 st[0x34]; u8 st2[0x40]; };
void sub_08220D78(void *, void *, s32, s32, s32);
s32 sub_08220F70(void *, void *);
void sub_081922CC(struct P *p, s32 i)
{
    u8 *s;
    u8 *t = (u8 *)p + 0x71DC;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].fl &= ~1;
        sub_08220D78(&p->rows[i], p->hdr, 0x30, 2, 4);
        (*s)++;
        p->st2[i] = z;
    }
    if (sub_08220F70(&p->rows[i], p->hdr) != 0) {
        *s = 0xff;
        p->st2[i] = 1;
    }
}
