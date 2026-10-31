#ifndef ANIMATED_HAZARD_H
#define ANIMATED_HAZARD_H
#include "engine.h"
#include "scene_vec.h"

// Tentative: a task owning eight ground hazards (spikes/geysers). Each one waits (or reacts to the
// player coming close), emits a warning, plays attack/hold/retraction animations while its
// collider hurts, then returns to idle. state: 0 idle, 1 proximity wait, 2 warning, 3 attack,
// 4 hold, 5 retraction (guess).

struct HazardSprite {
    u32 flags; // bit 0: hidden, bit 2/3: flip (from the animation)
    u8 pad4[12];
    u16 frame;
    u8 pad12[10];
    struct SceneVec position;
    u8 tail[8];
};

struct HazardExtent {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

struct HazardAnim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};

struct AnimatedHazard {
    u8 active, state, entered, pad3;
    u16 duration, pad6;
    u32 timer;
    u32 hold;
    u8 collider[84];
    struct HazardSprite sprite;
    struct HazardAnim anim;
};

struct AnimatedHazardManager {
    struct Task task;
    u8 animData[28];
    void *script;
    struct AnimatedHazard hazards[8];
};

/* 0x02000050 */
extern struct AnimatedHazardManager *gAnimatedHazardManager;

struct SceneVec *Player_GetEffectivePosition(s32);
void Collider_SetAttackPowerAndForce(void *, s32, s32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void AnimatedParticles_EmitOffset(void *, s32, s32, s32, s32, s32, s32, s32);

#endif
