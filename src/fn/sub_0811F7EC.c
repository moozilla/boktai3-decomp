#include "global.h"
#ifdef NONMATCHING
// fuzzy: 82.8%
// foundry draft (worker best 82.8%); what differs is mostly register allocation and instruction order
#define ActorAction_TrySpawn sub_0811F7EC
struct ActionOwner {
    u8 unknown0[0x18];
    u8 actionState[0x10];
    u8 position[0x28];
    u8 direction[0x20];
    s16 resource;
    u8 unknown72[0x876];
    u32 spawnData;
    u8 unknown8EC[0xC];
    s8 spawnKind;
};
struct ActorAction {
    u8 unknown0[4];
    u8 parameters[4];
    u16 id;
    u8 unknownA[2];
    u16 duration;
    u8 unknownE[3];
    s8 spawnResult;
    u8 unknown12[2];
    u16 timer;
    u8 unknown16[2];
    struct ActionOwner *owner;
};
u8 Modifier_IsFourthSourceReady(struct ActorAction *);
s32 CallbackList_RunMatchingState(void *, u32, s32);
void CallbackList_SubtractStateTenValue(void *, struct ActorAction *, s32);
struct SpawnParameters { u32 data; s32 a, b; };
s8 SpritePairPool_StartEntry(void *, void *, s32, void *, s32, struct SpawnParameters);
s32 sub_0811F7EC(struct ActorAction *action)
{
    struct ActionOwner *owner;
    void *state;
    s32 cost;

    if (!Modifier_IsFourthSourceReady(action)) goto fail;
    owner = action->owner;
    state = owner->actionState;
    cost = CallbackList_RunMatchingState(state, action->id, 10);
    if (owner->resource < cost) goto fail;
    action->spawnResult = SpritePairPool_StartEntry(owner->position, owner->direction, 0, action->parameters,
                                      owner->spawnKind, (struct SpawnParameters){owner->spawnData, 0, 0});
    if (action->spawnResult < 0) {
fail:
        return 0;
    }
    CallbackList_SubtractStateTenValue(state, action, cost);
    action->timer = action->duration;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0811F7EC);
#endif
