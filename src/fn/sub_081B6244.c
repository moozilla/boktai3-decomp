#include "paired_scene_actor.h"

s32 PairedSceneActor_ApplyToSelectedHalves(struct VanargandAttackPair *p, u32 mask, u32 value)
{
    if (!p)
        return 0;
    if (mask & 1) {
        Vanargand_ResetState(&p->halves[0], value);
        if (PairedSceneHalf_IsAlive(&p->halves[0])) {
            u32 bit = 0x100;
            if (!(p->halves[0].actionFlags & bit)) {
                p->halves[0].state = value;
                p->halves[0].finished = 0;
                p->selectedMask |= 1;
            }
        }
    }
    if (mask & 2) {
        Vanargand_ResetState(&p->halves[1], value);
        if (PairedSceneHalf_IsAlive(&p->halves[1])) {
            u32 bit = 0x100;
            if (!(p->halves[1].actionFlags & bit)) {
                p->halves[1].state = value;
                p->halves[1].finished = 0;
                p->selectedMask |= 2;
            }
        }
    }
    return p->selectedMask;
}
