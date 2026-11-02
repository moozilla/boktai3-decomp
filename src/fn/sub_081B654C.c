#include "paired_scene_actor.h"

u32 PairedSceneActor_GetActiveMask(struct VanargandAttackPair *p)
{
    u32 mask = 0;
    if (PairedSceneHalf_IsAlive(&p->halves[0]))
        mask |= 1;
    if (PairedSceneHalf_IsAlive(&p->halves[1]))
        mask |= 2;
    return mask;
}
