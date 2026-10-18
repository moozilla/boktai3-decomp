#include "tether_actor.h"
/* TetherActor_GetStatusTag: Returns the actor status tag at offset 0x3E6 or zero when no actor exists */
#define TetherActor_GetStatusTag sub_08049F54
#define TetherActor_GetStatusTag sub_08049F54

extern u8 *gUnk_02000488;

u16 TetherActor_GetStatusTag(void)
{
    if (gUnk_02000488 == NULL)
        return 0;
    return *(u16 *)(gUnk_02000488 + 0x3e6);
}
