#include "global.h"
extern u8 *gUnk_02000710;
s32 Solar_GetLevel(void)
{
    return *(s16 *)(gUnk_02000710 + 0x876);
}
