#ifndef GUARD_CONTACT_SEQUENCE_POOL_H
#define GUARD_CONTACT_SEQUENCE_POOL_H

#include "contact_sequence_actor.h"

struct ContactSequenceFrame {
    u16 frame, unknown2;
};

struct ContactSequenceScriptArgs {
    u32 count : 16;
    u32 unused : 16;
    s32 *values;
};

void ContactSequenceActor_SetFlags(struct ContactSequenceActor *actor, u32 flags);
void ContactSequenceActor_ClearFlags(struct ContactSequenceActor *actor, u32 mask);
u32 ContactSequenceActor_GetFlags(struct ContactSequenceActor *actor, u32 mask);
void ContactSequenceActor_SetState(struct ContactSequenceActor *actor, u32 state);
u32 ContactSequenceActor_TakeEntryFlag(struct ContactSequenceActor *actor);
void ContactSequenceActor_ApplyHealthContact(struct ContactCollider *source,
                                             struct ContactCollider *other,
                                             struct ContactSequenceActor *actor);
void ContactSequenceActor_Initialize(struct ContactSequenceOwner *owner,
                                     struct ContactSequenceActor *actor);
void ContactSequenceActor_Deactivate(struct ContactSequenceOwner *owner,
                                     struct ContactSequenceActor *actor);
void ContactSequenceActor_UpdateInactive(struct ContactSequenceOwner *owner,
                                         struct ContactSequenceActor *actor);
void ContactSequenceActor_UpdateAppearing(struct ContactSequenceOwner *parent,
                                          struct ContactSequenceActor *owner);
void ContactSequenceActor_UpdateActive(struct ContactSequenceOwner *parent,
                                       struct ContactSequenceActor *owner);
void ContactSequenceActor_UpdateDying(struct ContactSequenceOwner *parent,
                                      struct ContactSequenceActor *actor);
void ContactSequenceActor_Update(struct ContactSequenceOwner *parent,
                                 struct ContactSequenceActor *actor);
void ContactSequenceActor_Destroy(struct ContactSequenceOwner *owner,
                                  struct ContactSequenceActor *actor);
s32 ContactSequencePool_Update(struct ContactSequenceOwner *owner);
s32 ContactSequencePool_Destroy(struct ContactSequenceOwner *owner);
s32 ContactSequencePool_Initialize(struct ContactSequenceOwner *owner, s32 initialHealth,
                                   u32 contactValue, s32 resourceValue, u32 script,
                                   u32 reactionValue);
struct ContactSequenceOwner *ContactSequencePool_Create(s32 initialHealth, u32 contactValue,
                                                        s32 resourceValue, u32 script,
                                                        u32 reactionValue);
s32 ContactSequencePool_SpawnActor(struct ContactSequenceOwner *pool,
                                   const struct ContactPosition *position, u32 variant);
s32 ContactSequencePool_BeginAllDeaths(struct ContactSequenceOwner *owner);
u32 ContactSequencePool_GetActiveCount(struct ContactSequenceOwner *owner);

#endif
