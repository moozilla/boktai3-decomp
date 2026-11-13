#ifndef GUARD_POSITIONED_EFFECT_H
#define GUARD_POSITIONED_EFFECT_H
#include "engine.h"
#include "vanargand_pair_attack.h"
#include "contact_swarm_sprite.h"
struct PositionedPackedVector {
    u32 x : 16, y : 16, z : 16;
};
struct PositionedParticleOffset {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
union PositionedColliderOffset {
    struct PositionedPackedVector bits;
    struct ContactPosition vector;
};
struct PositionedEffectAnimation {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x50];
    struct ContactPosition *offset;
};
struct PositionedVectorEffect {
    u8 unknown0[0x20];
    struct ContactPosition position;
    u8 unknown28[0x58];
    struct ContactPosition source, shifted;
    u8 unknown90[0x18];
    struct PositionedEffectAnimation animation;
    struct ContactPosition base;
};
struct PositionedArc {
    struct ContactSwarmSprite nodes[6];
    u8 unknown108[0x1C];
    struct ContactPosition *start, *end;
};
void PositionedEffect_OnAttackContact(void);
void PositionedEffect_ReceiveContact(struct ContactCollider *, struct ContactCollider *,
                                     struct VanargandPairAttack *);
void PositionedEffect_InitContactCollider(struct VanargandPairAttack *);
void PositionedEffect_InitAttackCollider(struct VanargandPairAttack *);
void PositionedEffect_InitRadialAttackColliders(struct VanargandPairAttack *);
void PositionedEffect_EmitParticles(struct VanargandPairAttack *);
void Vanargand_RunUnusedHook(void);
void Debug_TestIsometricProjection(void);
void ScreenPalette_ProjectPosition(u32, struct ContactPosition *, s32, s32);
void ScreenPalette_ProjectPositionAt88(struct PositionedEffectCoordinates *);
void PositionedEffect_UpdateVectors(struct PositionedVectorEffect *);
void PositionedEffect_InterpolateArcNodes(struct PositionedArc *);
void PositionedEffect_SetHalfScaleCoordinates(struct PositionedEffectCoordinates *,
                                              struct ContactPosition *);
void PositionedEffect_UpdateOscillatingCoordinates(struct VanargandPairAttack *, s32);
#endif
