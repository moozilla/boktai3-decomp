#include "global.h"
extern u8 *gUnk_030042E4;
u8 *sub_08019E58(s32 i)
{
    return gUnk_030042E4 + (i + 0x373) * 0x20;
}
