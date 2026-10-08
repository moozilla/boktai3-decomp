#include "global.h"

extern u8 gUnk_08E60F4C[];

u8 *sub_0814DCD0(s32 idx)
{
    return gUnk_08E60F4C + ((idx - 1) << 2);
}
