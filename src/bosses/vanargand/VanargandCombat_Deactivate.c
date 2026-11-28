#include "bosses/vanargand/paired_scene_actor.h"
#include "global.h"
void VanargandCombat_Deactivate(struct VanargandAttackPair *owner, struct PairedSceneHalf *status)
{
    if (!PairedSceneHalf_TestActionFlags(status, 0x100)) {
        u32 previous = (u8)(status->requestedState - 0x19);
        u8 *phase = &status->finished;
        if (previous > 1 || *phase) {
            status->requestedState = 0;
            status->state = 0;
            status->entered = 1;
            *phase = 0;
            status->stateTimer = 0;
            status->actionCounters[0] = 0;
            status->actionCounters[1] = 0;
        }
    }
    status->sprite.flags |= 1;
    if (status->mode == 0)
        owner->selectedMask &= ~1;
    else
        owner->selectedMask &= ~2;
    status->health = 0;
}
