#include "global.h"
extern u32 gUnk_02000274;
void sub_08020D08(u8 *);
void sub_08163E68(u32);
void sub_0803F7D8(u8 *);
s32 sub_081D0B8C(u8 *a, u8 *b)
{
    u8 *q = b + 0x20;
    sub_08020D08(b + 0x168);
    sub_08163E68(*(u32 *)(q + 4));
    if (q[0xd] != 0) sub_0803F7D8(b + 0x344);
    gUnk_02000274 = 0;
}
