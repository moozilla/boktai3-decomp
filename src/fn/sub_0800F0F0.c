#include "global.h"
void CpuSet(const void *, void *, u32);
void sub_0800F0F0(u8 *p) {
    u32 zero;
    u16 index;
    u32 offset;
    volatile u32 *dest;
    zero = 0;
    CpuSet(&zero, p + 0x2C, 0x05000091);
    dest = (volatile u32 *)0x03004278;
    index = *(u16 *)(p + 0x2AC);
    offset = index;
    offset <<= 2;
    offset += index;
    offset <<= 6;
    offset += 0x2C;
    *dest = (u32)(p + offset);
    *(volatile u32 *)0x03003A00 = 0x0400004C;
    *(volatile u16 *)0x03004270 = 1;
    *(volatile u16 *)0x03004274 = 0;
    *(u32 *)(p + 0x2D4) = 0;
}
