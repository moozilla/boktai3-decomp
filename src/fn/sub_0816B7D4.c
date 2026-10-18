#include "global.h"
extern u8 *gUnk_02000710;
u32 GameState_GetMapRevealFlag(u32 n)
{
    return *(u32 *)(gUnk_02000710 + 0x5b8) & (1 << n);
}
