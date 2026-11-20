#include "paired_scene_actor.h"
u8 Vanargand_IsEitherComponentFlag40Set(u8 *p)
{
    struct VanargandAttackPair *pair = (struct VanargandAttackPair *)p;
    u32 m = 0x40;
    if ((pair->halves[0].actionFlags & m) || (pair->halves[1].actionFlags & m))
        return 1;
    return 0;
}
