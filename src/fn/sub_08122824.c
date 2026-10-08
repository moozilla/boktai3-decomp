#include "global.h"
struct S { u8 f[0x130]; s16 a; u8 g[0x3c - 2 + 0x2]; u16 c; };
u8 sub_0812266C(void);
void sub_0821FF24(void *, void *, u32);
void sub_0821FE40(void *);
void sub_08122824(u8 *s)
{
    if (sub_0812266C() == 0) {
        if (*(s16 *)(s + 0x130) == 1) {
            u8 *p = s + 0xdc;
            sub_0821FF24(p, s + 0xd4, 0);
            sub_0821FE40(p);
        }
        (*(u16 *)(s + 0x13c))++;
    }
}
