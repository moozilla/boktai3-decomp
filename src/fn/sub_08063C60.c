#include "roaming_object.h"
void RoamingObject_UpdateAnimation(struct RoamingObject *);
void sub_08225B74(void *);
u32 RoamingObject_Tick(struct RoamingObject *owner)
{
    sub_0824923C(owner, (void (*)(void *))owner->update);
    RoamingObject_UpdateAnimation(owner);
    sub_08225B74((u8 *)owner + 0x1C);
    return 0;
}
