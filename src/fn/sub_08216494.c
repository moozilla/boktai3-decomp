#include "global.h"

struct Bg {
    u8 pad0[0x10];
    u16 f10, f12;
    u8 pad14[0x1C];
};
extern struct Bg gUnk_03004C30[];

void sub_08216494(u32 n)
{
    struct Bg *bg = &gUnk_03004C30[n];
    bg->f10 = 0x1000;
    bg->f12 = 0x1000;
}
