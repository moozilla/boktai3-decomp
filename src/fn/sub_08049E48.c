#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049E48(u32 a)
{
    u8 *p = gUnk_02000488;
    if (p) *(s16 *)(p + 0x408) = a;
}
