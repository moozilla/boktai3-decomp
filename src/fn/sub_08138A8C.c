#include "global.h"
extern u8 *gUnk_020004C0;
void sub_08138A8C(u32 i)
{
    u8 *b = gUnk_020004C0;
    if (b != 0 && i <= 3) {
        u8 *e = b + i * 0x60;
        if (*(u16 *)(e + 0x76) != 0)
            *(u16 *)(e + 0x74) = 1;
    }
}
