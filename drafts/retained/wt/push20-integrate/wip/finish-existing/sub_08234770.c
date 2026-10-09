#include "global.h"
struct Pair77 { u32 a, b; };
void sub_08234540(u8 *);
void sub_082346EC(u8 *);
s32 Div(s32, s32);
void sub_0822B2F8(u32);
void sub_082343B8(u8 *);
void sub_082343A0(u8 *, void (*)(u8 *));
s32 sub_08234770(u8 *p, u8 *owner, struct Pair77 *q)
{
    s32 value;
    *(struct Pair77 *)(p + 0x1c) = *q;
    *(u8 **)(p + 0x18) = owner;
    sub_08234540(p);
    sub_082346EC(p);
    value = (*(u16 *)(p + 0x1fe) = 180);
    *(u16 *)(p + 0x1fc) = Div(value, 3);
    value = (*(u16 *)(p + 0x200) = 900);
    sub_0822B2F8(value + 19);
    sub_082343A0(p, sub_082343B8);
    return 0;
}
