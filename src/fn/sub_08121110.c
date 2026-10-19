
#ifdef NONMATCHING
// fuzzy: 90.6%
// foundry draft (worker best 87.9%); what differs is mostly register allocation and instruction order
#define ActorAction_RunActiveCallbacks sub_08121110
#define ActorAction_ResetFlags         RegistryA8CB_ResetUpdateState
#define ActorAction_FinishTick         RegistryA8CB_UpdateAnimation
#include "global.h"
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
s32 sub_08121110(struct ActorActionRunner *actor)
{
    struct ActorActionEntry *action = &actor->action;
    struct ActorActionRunner *object = (struct ActorActionRunner *)actor->object;
    s32 i = 0;
    do {
        if (action->active == 1) {
            RegistryA8CB_ResetUpdateState(object);
            Callback_InvokeActor(object, action->callback);
            RegistryA8CB_UpdateAnimation(object);
        }
        action++;
        object++;
    } while (--i >= 0);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08121110);
#endif
