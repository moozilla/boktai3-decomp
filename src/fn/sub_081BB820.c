#include "global.h"
void sub_0822B2F8(u32);
void sub_08220F70(u8 *, u8 *);
void sub_081BB5CC(u8 *);
void sub_081BB820(u8 *p)
{
    u16 *c = (u16 *)(p + 0x70c);
    if (*c == 0x20) sub_0822B2F8(0x2be);
    sub_08220F70(p + 0x58, p + 0x18);
    (*c)++;
    if (*c > 0x68) sub_081BB5CC(p);
}
