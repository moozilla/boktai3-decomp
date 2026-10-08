#include "global.h"
u32 sub_08066DB8(u32, u8 *, u32, u32, u32, u32, u32, u32);
void sub_0804AAB4(u8 *p, u32 a)
{
    u16 *q = (u16 *)(p + 0x386);
    u32 v = *q + a;
    *q = v;
    if (*(u16 *)(p + 0x384) == 0) {
        if ((u16)v > 0x4B0) {
            *q = 0x4B0;
            if (*(u16 *)(p + 0x3FC) == 0) {
                if (p[0x255] != 2) {
                    *(u16 *)(p + 0x384) = 1;
                    *(u32 *)(p + 0x47C) = sub_08066DB8(*(u32 *)(p + 0x47C), p + 0x44, 1, 0, (u32)q, 0x4B0, 0xFA, 0x122);
                }
            }
        }
    } else {
        if ((u16)v > 0x4B0) *q = 0x4B0;
    }
}
