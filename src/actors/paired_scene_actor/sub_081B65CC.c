#include "actors/paired_scene_actor/paired_scene_actor.h"
s32 Vanargand_GetComponentWithFlag10(u8 *p)
{
    struct VanargandAttackPair *pair = (struct VanargandAttackPair *)p;
    u32 m = 0x10;
    if (pair->halves[0].actionFlags & m)
        return 0;
    if (pair->halves[1].actionFlags & m)
        return 1;
    return -1;
}
