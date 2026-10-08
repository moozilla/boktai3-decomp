#include "global.h"
struct T { u32 lo:16; u32 hi:16; u32 lo2:16; u32 hi2:16; };
void sub_0811D384(void *);
void sub_0811C9F8(void *, void *, u32);
void sub_0811CB68(void *, void *, void *, s32);
void sub_0811CD24(void *, s32);
void sub_0811CD5C(void *, s32);
void sub_0811C844(void *, u32);
void sub_0811202C(u8 *s)
{
    struct T t;
    u8 *a;
    u8 *b;
    sub_0811D384(s + 0x9B0);
    s[0x11B2] = 0;
    t.lo = 0x30;
    t.hi = 0x50;
    t.lo2 = 0;
    a = s + 0x968;
    b = s + 0xF60;
    sub_0811C9F8(a, b, 0x3D99);
    sub_0811CB68(a, b, &t, 0);
    sub_0811CD24(b, 0);
    sub_0811CD5C(b, 0);
    sub_0811C844(b, s[0x9B3]);
}
