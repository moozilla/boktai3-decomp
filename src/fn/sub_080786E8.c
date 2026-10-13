#include "global.h"

void Shadow_Initialize(u8 *, u8 *, u32);

void sub_080786E8(u8 *p)
{
    Shadow_Initialize(p + 0x320, p + 0x5c, 0);
}
