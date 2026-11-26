#ifndef GUARD_CONTACT_SEQUENCE_POOL_H
#define GUARD_CONTACT_SEQUENCE_POOL_H

#include "bosses/nidhoggr/nidhoggr_duel_minion.h"

struct ContactSequenceFrame {
    u16 frame, unknown2;
};

struct ContactSequenceScriptArgs {
    u32 count : 16;
    u32 unused : 16;
    s32 *values;
};

void NidhoggrDuelMinion_SetFlags(struct ContactSequenceActor *actor, u32 flags);
void NidhoggrDuelMinion_ClearFlags(struct ContactSequenceActor *actor, u32 mask);
u32 NidhoggrDuelMinion_GetFlags(struct ContactSequenceActor *actor, u32 mask);
void NidhoggrDuelMinion_SetState(struct ContactSequenceActor *actor, u32 state);
u32 NidhoggrDuelMinion_TakeEntryFlag(struct ContactSequenceActor *actor);
void NidhoggrDuelMinion_ApplyHealthContact(struct ContactCollider *source,
                                             struct ContactCollider *other,
                                             struct ContactSequenceActor *actor);
void NidhoggrDuelMinion_Initialize(struct ContactSequenceOwner *owner,
                                     struct ContactSequenceActor *actor);
void NidhoggrDuelMinion_Deactivate(struct ContactSequenceOwner *owner,
                                     struct ContactSequenceActor *actor);
void NidhoggrDuelMinion_UpdateInactive(struct ContactSequenceOwner *owner,
                                         struct ContactSequenceActor *actor);
void NidhoggrDuelMinion_UpdateAppearing(struct ContactSequenceOwner *parent,
                                          struct ContactSequenceActor *owner);
void NidhoggrDuelMinion_UpdateActive(struct ContactSequenceOwner *parent,
                                       struct ContactSequenceActor *owner);
void NidhoggrDuelMinion_UpdateDying(struct ContactSequenceOwner *parent,
                                      struct ContactSequenceActor *actor);
void NidhoggrDuelMinion_Update(struct ContactSequenceOwner *parent,
                                 struct ContactSequenceActor *actor);
void NidhoggrDuelMinion_Destroy(struct ContactSequenceOwner *owner,
                                  struct ContactSequenceActor *actor);
s32 NidhoggrDuelMinionPool_Update(struct ContactSequenceOwner *owner);
s32 NidhoggrDuelMinionPool_Destroy(struct ContactSequenceOwner *owner);
s32 NidhoggrDuelMinionPool_Initialize(struct ContactSequenceOwner *owner, s32 initialHealth,
                                   u32 contactValue, s32 resourceValue, u32 script,
                                   u32 reactionValue);
struct ContactSequenceOwner *NidhoggrDuelMinionPool_Create(s32 initialHealth, u32 contactValue,
                                                        s32 resourceValue, u32 script,
                                                        u32 reactionValue);
s32 NidhoggrDuelMinionPool_SpawnActor(struct ContactSequenceOwner *pool,
                                   const struct ContactPosition *position, u32 variant);
s32 NidhoggrDuelMinionPool_BeginAllDeaths(struct ContactSequenceOwner *owner);
u32 NidhoggrDuelMinionPool_GetActiveCount(struct ContactSequenceOwner *owner);

#endif
