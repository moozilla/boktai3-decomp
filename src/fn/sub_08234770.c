#include "global.h"
#ifdef NONMATCHING
// fuzzy: 85.4%
// 5 differing instruction runs; size +8 bytes; first diff at +0x22 (movs r1, #3 vs movs r0, #0xb4)
struct Pair77 {
    u32 a, b;
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
    *(u16 *)(p + 0x1fe) = 180;
    *(u16 *)(p + 0x1fc) = Div(*(u16 *)(p + 0x1fe), 3);
    *(u16 *)(p + 0x200) = 900;
    Sound_PlaySong(919);
    sub_082343A0(p, sub_082343B8);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08234770);
#endif
