#include "paired_scene_actor.h"

u32 PairedSceneActor_AreSelectedHalvesFinished(struct VanargandAttackPair *p)
{
    if (p->selectedMask & 1) {
        if (PairedSceneHalf_IsAlive(&p->halves[0])) {
            if (!p->halves[0].finished)
                return 0;
        }
    }
    if (p->selectedMask & 2) {
        if (PairedSceneHalf_IsAlive(&p->halves[1])) {
            if (!p->halves[1].finished)
                return 0;
        }
    }
    return 1;
}
