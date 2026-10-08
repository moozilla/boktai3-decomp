#include "global.h"
struct E { u8 f[0x60]; };
struct S { u8 f[0x24]; struct E e[8]; };
void sub_08219728(void *, void *, u16);
void sub_0810FEDC(struct S *s, s32 k, s32 v)
{
    s32 idx = k * 4 + 1;
    u32 d = 0x3e;
    while (v > 999) { d++; v -= 1000; }
    sub_08219728(&s->e[idx], (u8 *)s + 0x704, d);
    d = 0x3e;
    while (v > 99) { d++; v -= 100; }
    sub_08219728(&s->e[idx + 1], (u8 *)s + 0x704, d);
    d = 0x3e;
    while (v > 9) { d++; v -= 10; }
    sub_08219728(&s->e[idx + 2], (u8 *)s + 0x704, d);
    d = 0x3e;
    while (v > 0) { d++; v -= 1; }
    sub_08219728(&s->e[idx + 3], (u8 *)s + 0x704, d);
}
