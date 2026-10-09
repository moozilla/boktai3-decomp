#include "global.h"
struct Pair33A { u32 a, b; };
void sub_082339C8(u8 *);
void sub_08233A1C(u8 *);
void sub_08233924(u8 *, u32, u32, u32);
void sub_0822B2F8(u32);
void sub_08233714(u8 *);
void sub_082334F4(u8 *, void (*)(u8 *));
s32 sub_08233AA0(u8 *p, struct Pair33A *q, u32 a, u32 b, u32 c, u32 d)
{
    *(struct Pair33A *)(p + 0x18) = *q;
    sub_082339C8(p);
    sub_08233A1C(p);
    sub_08233924(p, a, b, c);
    *(u16 *)(p + 0x206) = 0;
    *(u16 *)(p + 0x208) = d;
    sub_0822B2F8(920);
    sub_082334F4(p, sub_08233714);
    return 0;
}
