#include "global.h"
struct P { u8 pad[0x24]; u32 w24; };
void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);
void sub_08177DEC(u32);
void sub_0821447C(u32, void *, void *, void *);
void sub_0821AD08(u32, u32);
void sub_08217F4C(void);
void sub_08214B44(void);
void sub_08219624(void);
void sub_08038FE8(u8 *p)
{
    u8 *f = p + 0x29;
    if (*f != 0) {
        u32 z = 0;
        *f = z;
        sub_082279A8(3, 3, 0, 0, z, 0xFFFF, z);
    }
    if (*(u32 *)(p + 0x24) == 10) {
        sub_08177DEC(7);
        sub_0821447C(0, sub_08217F4C, sub_08214B44, sub_08219624);
        if (*(u32 *)(p + 0xd4) != 0) sub_0821AD08(*(u32 *)(p + 0xd4), 0);
    }
    *(u32 *)(p + 0x24) += 1;
}
