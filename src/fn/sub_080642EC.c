#include "roaming_object.h"
void RoamingObjectB_UpdateAnimation(struct RoamingObject *);
void sub_08225B74(void *);
u32 RoamingObjectB_Tick(struct RoamingObject *owner)
{
    sub_0824923C(owner, (void (*)(void *))owner->update);
    RoamingObjectB_UpdateAnimation(owner);
    sub_08225B74((u8 *)owner + 0x1C);
    return 0;
}
