#define SceneActor_UpdateDescriptorApproach sub_080D5FD8
#include "global.h"
#include "scene_actor_behavior.h"
struct SceneActorApproachParameters {
    u8 unknown0[0x1FC];
    struct SceneActorBehaviorDestination *destination;
};
struct SceneActorApproach {
    void *object, *drawable;
    u8 kind;
    u8 unknown9[0x257];
    void *speedCallback;
    u8 unknown264[12];
    void *updateCallback;
    u8 unknown274[0x34];
    u8 phase, step, state, unknown2AB;
    s32 timer;
    u8 substate, resetRequested;
    u8 unknown2B2[0x10];
    u16 palette[1];
    u8 unknown2C4[0x10];
    u16 flags;
    u8 unknown2D6[0xFE];
    u8 parameters[0];
};
u8 SceneActorBehavior_AccelerateTowardDescriptor(struct SceneActorBehaviorView *,
                                                 struct SceneActorBehaviorDestination *, u32, u32,
                                                 s32);
u8 SceneActorBehavior_RequestDescriptorTurn(struct SceneActorBehaviorView *,
                                            struct SceneActorBehaviorDestination *, u32, u32, s32);
u32 sub_0821F840(void *);
void sub_08220D44(void *, u32);
void sub_08220F00(void *, u32);
s32 sub_0824923C(void *, void *);
static inline void ResetState(u8 *actor, u8 state)
{
    struct SceneActorApproach *a = (void *)actor;
    a->phase = 0;
    a->step = 0;
    a->state = state;
    a->timer = 0;
    a->resetRequested = 1;
}
void sub_080D5FD8(struct SceneActorApproach *actor)
{
    struct SceneActorApproachParameters *parameters = (void *)actor->parameters;
    s32 speed;
    u32 isTurn;
    u16 *palette;
    u32 mask;
    if ((*(u8 *)((u8 *)actor + 0x2B1)))
        (*(u8 *)((u8 *)actor + 0x2B1)) = 0;
    if ((u8)sub_0824923C(actor, actor->updateCallback))
        return;
    speed = sub_0824923C(actor, actor->speedCallback);
    mask = 0xA02F;
    if (actor->flags & mask) {
        actor->palette[0] = 0x40;
        palette = actor->palette;
    } else {
        palette = actor->palette;
        if (!*palette)
            *palette = 0x40;
    }
    if (!actor->kind)
        sub_08220D44((u8 *)actor->drawable + 0x48, *palette);
    else
        sub_08220F00((u8 *)actor->drawable + 0x20, *palette);
    isTurn = sub_0821F840(parameters->destination);
    if (!isTurn) {
        if (SceneActorBehavior_AccelerateTowardDescriptor((void *)actor, parameters->destination,
                                                          0x20, 0x20, speed)) {
            u32 state = 0x12;
            ResetState((u8 *)actor, state);
            return;
        }
    } else {
        if (SceneActorBehavior_RequestDescriptorTurn((void *)actor, parameters->destination, 0x20,
                                                     0x20, speed)) {
            u32 state = 0x11;
            ResetState((u8 *)actor, state);
        }
    }
    (*(s32 *)((u8 *)actor + 0x2AC))++;
}
