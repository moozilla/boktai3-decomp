#include "roaming_object.h"
#define RoamingObject_Tick sub_08063C60
void sub_08063B24(struct RoamingObject *);
void sub_08225B74(void *);
u32 RoamingObject_Tick(struct RoamingObject *owner)
{
    sub_0824923C(owner, (void (*)(void *))owner->update);
    sub_08063B24(owner);
    sub_08225B74((u8 *)owner + 0x1C);
    return 0;
}
