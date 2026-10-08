#include "global.h"
extern u8 *gUnk_02000710;
void sub_0805C774(u8 *p)
{
    s32 v;
    if (p[0xc0f] == 0) v = *(s16 *)(gUnk_02000710 + 0x876);
    else v = p[0xc11];
    p[0xc10] = v;
}
