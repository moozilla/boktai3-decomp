#include "roaming_object.h"
#define RoamingObjectB_Tick sub_080642EC
void sub_080641B0(struct RoamingObject *);
void sub_08225B74(void *);
u32 RoamingObjectB_Tick(struct RoamingObject *owner)
{
    sub_0824923C(owner, (void (*)(void *))owner->update);
    sub_080641B0(owner);
    sub_08225B74((u8 *)owner + 0x1C);
    return 0;
}
