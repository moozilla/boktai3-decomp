#include "roaming_object.h"
void RoamingObjectB_UpdateAnimation(struct RoamingObject *);
void Obj_StepPosition(void *);
u32 RoamingObjectB_Tick(struct RoamingObject *owner)
{
    Callback_InvokeActor(owner, (void (*)(void *))owner->update);
    RoamingObjectB_UpdateAnimation(owner);
    Obj_StepPosition((u8 *)owner + 0x1C);
    return 0;
}
