#include "global.h"
struct Part {
    u32 flags;
    u8 pad[0x5c];
};
void sub_081BC2F0(u8 *p)
{
    s32 i;
    u32 bit = 0x80;
    u8 *q = p;
    for (i = 21; i >= 0; i--) {
        *(u32 *)(q + 0x60) |= bit;
        q += 0x60;
    }
    *(u32 *)(p + 0x898) |= 0x80;
}
