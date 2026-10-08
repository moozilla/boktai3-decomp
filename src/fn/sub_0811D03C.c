#include "global.h"
struct S { u8 f[0x524]; u16 b; u16 a; };
void sub_0822B358(u32);
void sub_0822B2F8(u32);
void sub_0811D03C(struct S *s)
{
    u16 *a = &s->a;
    u16 *b = &s->b;
    if (*a != *b) {
        sub_0822B358(0x87);
        sub_0822B358(0x88);
        sub_0822B358(0x89);
        sub_0822B358(0x8a);
        sub_0822B358(0x8b);
        sub_0822B2F8(*b);
        *a = *b;
    }
}
