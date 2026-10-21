#include "global.h"

#define ActorAction_ResetFlags RegistryA8CB_ResetUpdateState
#define ActorAction_FinishTick RegistryA8CB_UpdateAnimation
struct ActorActionEntry {
    void *callback;
    u8 unknown4[0x5c];
    s32 active;
    u8 unknown64[0x8a4];
};
struct ActorActionRunner {
    u8 unknown0[0x18];
    u8 object[0x854];
    struct ActorActionEntry action;
    u8 unknown8D0[0x38];
};
void RegistryA8CB_ResetUpdateState(void *);
void RegistryA8CB_UpdateAnimation(void *);
s32 Callback_InvokeActor(void *, void *);
s32 ActorAction_RunActiveCallbacks(struct ActorActionRunner *actor)
{
    struct ActorActionEntry *action = &actor->action;
    struct ActorActionEntry *object = (struct ActorActionEntry *)actor->object;
    s32 i = 0;
    do {
        if (action->active == 1) {
            RegistryA8CB_ResetUpdateState(object);
            Callback_InvokeActor(object, action->callback);
            RegistryA8CB_UpdateAnimation(object);
        }
        do {
            action++;
        } while (0);
        object++;
    } while (--i >= 0);
    return 0;
}
