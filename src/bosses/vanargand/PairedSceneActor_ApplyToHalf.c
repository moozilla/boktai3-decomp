#include "bosses/vanargand/paired_scene_actor.h"

static inline void ApplyState(struct PairedSceneHalf *half, u32 value)
{
    if (!PairedSceneHalf_TestActionFlags(half, 0x100)) {
        half->state = value;
        half->finished = 0;
    }
}

void PairedSceneActor_ApplyToHalf(struct VanargandAttackPair *p, u32 index, u32 value)
{
    if (p && index < 2) {
        Vanargand_ResetState(&p->halves[index], value);
        if (PairedSceneHalf_IsAlive(&p->halves[index])) {
            ApplyState(&p->halves[index], value);
        }
    }
}
