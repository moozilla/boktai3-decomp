#include "global.h"
struct Pair77 {
    u32 a;
    u32 b;
};
void sub_08234540(u8 *);
void sub_082346EC(u8 *);
s32 Div(s32, s32);
void Sound_PlaySong(u32);
void sub_082343B8(u8 *);
void sub_082343A0(u8 *, void (*)(u8 *));
s32 sub_08234770(u8 *p, u8 *owner, struct Pair77 *q)
{
    *(struct Pair77 *)(p + 0x1c) = *q;
    *(u8 **)(p + 0x18) = owner;
    sub_08234540(p);
    sub_082346EC(p);
    {
        u16 a;
        *(u16 *)(p + 0x1fe) = a = 180;
        *(u16 *)(p + 0x1fc) = Div(a, 3);
    }
    {
        u16 b;
        *(u16 *)(p + 0x200) = b = 900;
        Sound_PlaySong(b + 19);
    }
    sub_082343A0(p, sub_082343B8);
    return 0;
}
