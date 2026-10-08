#include "global.h"

u32 sub_0806143C(u8 *);

u32 sub_08061CA8(u8 *p)
{
    u32 r = 0;
    if (*(u8 *)(p + 0x1AA7) == 3) {
        if (*(u8 *)(p + 0x1AB0) < *(u8 *)(p + 0x1AB2))
            *(u8 *)(p + 0x1AA8) = 1;
        else
            *(u8 *)(p + 0x1AA8) = 2;
        r = sub_0806143C(p);
        *(u16 *)(p + 0x1AEC) = r;
    }
}
