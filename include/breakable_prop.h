#ifndef GUARD_BREAKABLE_PROP_H
#define GUARD_BREAKABLE_PROP_H
#include "contact_swarm.h"
#include "game_state.h"
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
s32 AnimatedParticles_Create(s32, void *, struct AnimatedParticleConfig *);
extern u32 gUnk_03005308;
s32 Mod(s32, s32);
void sub_0805175C(struct BreakableProp *, void *);
void sub_080517D0(struct BreakableProp *, u32, void *);
u8 *sub_0821D64C(s32, s32);
void sub_0821D698(void *, u32, u32, u32, u32, u32);
#endif
