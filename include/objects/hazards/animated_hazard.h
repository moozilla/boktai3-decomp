#ifndef ANIMATED_HAZARD_H
#define ANIMATED_HAZARD_H
#include "engine/engine.h"
#include "scene/actor/contact_collider.h"
#include "engine/video/display_sequence.h"
#include "engine/math/scene_vec.h"
#include "engine/video/sprite.h"

// Tentative: a task owning eight ground hazards (spikes/geysers). Each one waits (or reacts to the
// player coming close), emits a warning, plays attack/hold/retraction animations while its
// collider hurts, then returns to idle. state: 0 idle, 1 proximity wait, 2 warning, 3 attack,
// 4 hold, 5 retraction (guess).

struct HazardExtent {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

struct AnimatedHazard {
    u8 active, state, entered, pad3;
    u16 duration, pad6;
    u32 timer;
    u32 hold;
    struct ContactCollider collider;
    struct DisplaySequenceSprite sprite;
    struct DisplaySequenceState anim;
};

struct AnimatedHazardManager {
    struct Task task;
    struct SpriteRenderResource animData;
    void *script;
    struct AnimatedHazard hazards[8];
};

/* 0x02000050 */
extern struct AnimatedHazardManager *gAnimatedHazardManager;

struct ContactPosition *Player_GetEffectivePosition(s32);
void Collider_SetAttackPowerAndForce(void *, s32, s32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void AnimatedParticles_EmitOffset(void *, s32, s32, s32, s32, s32, s32, s32);

#endif
