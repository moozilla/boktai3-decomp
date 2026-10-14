#include "tether_actor.h"
#define TetherActor_GetStatusTag sub_08049F54

extern u8 *gUnk_02000488;

u16 TetherActor_GetStatusTag(void)
{
    if (gUnk_02000488 == NULL)
        return 0;
    return *(u16 *)(gUnk_02000488 + 0x3e6);
}
