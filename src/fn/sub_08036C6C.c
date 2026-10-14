#include "global.h"

extern u8 *gUnk_02000484;

u8 *sub_08036C6C(void)
{
    if (gUnk_02000484 == NULL)
        return NULL;
    return gUnk_02000484 + 0x50;
}
