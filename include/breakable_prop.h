#ifndef GUARD_BREAKABLE_PROP_H
#define GUARD_BREAKABLE_PROP_H
#include "contact_swarm.h"
#include "game_state.h"
#include "engine_globals.h"
/* Tentative identity: cell-centered object removed by a flagged contact. */
struct BreakableProp {
    u8 task[0x18];
    struct ContactSwarmSprite sprite;
    u8 graphics[0x1C];
    struct ContactCollider collider;
    u8 terrainRegistration[0x10];
    u8 unknownC4, height;
    u16 cell;
    u8 destroyRequested, unknownC9[3];
    u32 scriptId;
};
struct AnimatedParticleConfig {
    /* The particle consumer does not read the unused halfword at offset 4. */
    u16 type, flags, unused, speed;
    s16 vertical, verticalRange, horizontal, horizontalRange;
    u16 pitch, pitchRange, yaw, yawRange;
    u16 verticalStart, verticalStartRange, verticalDecay, verticalDecayRange;
    u16 horizontalStart, horizontalStartRange, horizontalDecay, horizontalDecayRange;
};
struct BreakablePropContact {
    u8 unknown0[0x34];
    u32 flags;
};
void BreakableProp_RequestBreakOnContact(struct BreakablePropContact *, s32,
                                         struct BreakableProp *);
void BreakableProp_EmitBreakParticles(struct BreakableProp *);
u32 BreakableProp_Tick(struct BreakableProp *);
u32 BreakableProp_Destroy(struct BreakableProp *);
s32 BreakableProp_InitializeFromScript(struct BreakableProp *, u32, u32);
s32 AnimatedParticles_Create(s32, void *, struct AnimatedParticleConfig *);
s32 Mod(s32, s32);
void BreakableProp_InitializeSprite(struct BreakableProp *, void *);
void BreakableProp_InitializeCollider(struct BreakableProp *, u32, void *);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void TerrainModifiers_RegisterCell(void *, u32, u32, u32, u32, u32);
#endif
