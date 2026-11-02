#include "paired_scene_actor.h"

u32 ScenePair_CountPositiveCounters(struct VanargandAttackPair *p)
{
    u32 count = 0;
    if (PairedSceneHalf_IsAlive(&p->halves[0]))
        count++;
    if (PairedSceneHalf_IsAlive(&p->halves[1]))
        count++;
    return count;
}
