#ifndef GUARD_QUEST_BARRIER_H
#define GUARD_QUEST_BARRIER_H
#include "effects/particles/impact_burst.h"
#include "scene/collision/contact_swarm.h"
#include "system/game_state/game_state.h"
#include "engine/engine_globals.h"
#include "engine/task.h"
/* Tentative identity: cell-centered object removed by a flagged contact. */
struct BreakableProp {
    struct Task task;
    struct ContactSwarmSprite sprite;
    u8 graphics[0x1C];
    struct ContactCollider collider;
    u8 terrainRegistration[0x10];
    u8 unknownC4, height;
    u16 cell;
    u8 destroyRequested, unknownC9[3];
    u32 scriptId;
};
void QuestBarrier_RequestBreakOnContact(struct ContactCollider *, s32,
                                         struct BreakableProp *);
void QuestBarrier_EmitBreakParticles(struct BreakableProp *);
u32 QuestBarrier_Tick(struct BreakableProp *);
u32 QuestBarrier_Destroy(struct BreakableProp *);
s32 QuestBarrier_InitializeFromScript(struct BreakableProp *, u32, u32);
s32 ImpactBurst_Create(s32, void *, struct AnimatedParticleConfig *);
s32 Mod(s32, s32);
void QuestBarrier_InitializeSprite(struct BreakableProp *, void *);
void QuestBarrier_InitializeCollider(struct BreakableProp *, u32, void *);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void TerrainModifiers_RegisterCell(void *, u32, u32, u32, u32, u32);
#endif
