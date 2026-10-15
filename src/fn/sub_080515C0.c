#include "breakable_prop.h"
void BreakableProp_RequestBreakOnContact(struct BreakablePropContact *contact, s32 unused,
                                         struct BreakableProp *prop)
{
    u32 mask = 0x200;
    if (contact->flags & mask)
        prop->destroyRequested = 1;
}
