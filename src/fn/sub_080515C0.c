#include "breakable_prop.h"
void BreakableProp_RequestBreakOnContact(struct ContactCollider *contact, s32 unused,
                                         struct BreakableProp *prop)
{
    u32 mask = 0x200;
    if (contact->statRewardFlags & mask)
        prop->destroyRequested = 1;
}
