#include "objects/quest_barriers/quest_barrier.h"
void QuestBarrier_RequestBreakOnContact(struct ContactCollider *contact, s32 unused,
                                         struct BreakableProp *prop)
{
    u32 mask = 0x200;
    if (contact->statRewardFlags & mask)
        prop->destroyRequested = 1;
}
