#include "actors/paired_scene_actor/paired_scene_actor.h"

s32 ScenePair_CountPositiveCounters(struct VanargandAttackPair *p)
{
    s32 count = 0;
    if (PairedSceneHalf_IsAlive(&p->halves[0]))
        count++;
    if (PairedSceneHalf_IsAlive(&p->halves[1]))
        count++;
    return count;
}
