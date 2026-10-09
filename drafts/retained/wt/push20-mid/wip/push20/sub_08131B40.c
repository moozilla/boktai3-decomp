#include "global.h"
extern u32 gUnk_030053F4, gUnk_030053F0, gUnk_02000560;
void sub_0824923C(u8 *, void *);
static inline u32 test(u32 *p,u32 m) { return *p & m; }
struct S { u8 pad[0x284]; void **table; u8 pad2[0x20]; u8 b[4]; u32 word; u8 pad3; u8 end; };
void sub_08131B40(u8 *s)
{
    u32 mask = 0x400;
    if ((gUnk_030053F4 | gUnk_030053F0) & mask || test(&gUnk_02000560,4)) {
        u32 two = 2;
        u32 one = 1;
        u8 *p = s + 0x2A8;
        u32 zero = 0;
        *p = two;
        p = s + 0x2A9;
        *p = zero;
        ++p;
        *p = one;
        p += 2;
        *(u32 *)p = zero;
        s[0x2B1] = one;
    } else {
        u32 i = s[0x2AA];
        struct S *obj = (struct S *)s;
        sub_0824923C(s, obj->table[i]);
    }
}
