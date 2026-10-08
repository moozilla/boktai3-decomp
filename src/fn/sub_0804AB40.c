#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049168(u8 *, u32);
u32 sub_08066DB8(u32, u8 *, u32, u32, u32, u32, u32, u32);
void sub_0804AB40(u8 *p)
{
    u16 *q = (u16 *)(p + 0x386);
    u32 z;
    if (*q != 0) {
        u32 v = *q - 1;
        z = 0;
        *q = v;
        if ((u16)v == 1) {
            *(u16 *)(p + 0x384) = z;
            *q = z;
            *(u32 *)(p + 0x47C) = sub_08066DB8(*(u32 *)(p + 0x47C), p + 0x44, 0x7F, z, z, z, 0xFA, 0x122);
        }
    }
}
