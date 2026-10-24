#include "global.h"

// Remaining differences: temporary registers for field offsets and target coordinate loads.
// confidence=low: advances presentation and caches bearings and planar target distances.
#define Vanargand_UpdateAnimationAndTargets sub_081AD3E8
struct TargetVec {
    s16 x, y, z, pad;
};
struct TargetSprite {
    u8 pad0[12];
    struct TargetVec position;
};
struct TargetPlayer {
    u8 pad0[0x457];
    u8 state;
};
struct VanargandTargets {
    u8 pad0[0x48];
    struct TargetSprite sprite;
    u8 pad5C[0x9C - 0x5C];
    u32 flags;
    u8 padA0[0xC8 - 0xA0];
    s32 distances[1];
    u8 bearings[1];
    u8 padCD[0x10C - 0xCD];
    struct TargetPlayer *player;
    struct TargetVec *targets[1];
    u8 pad114[0x174 - 0x114];
    u8 projection[0x18C - 0x174];
    u8 animation[0x60];
    u8 drawable[0x20C - 0x1EC];
    struct TargetVec renderPosition;
    u8 pad214[0x2D0 - 0x214];
    u8 arc[0x3FC - 0x2D0];
    u8 part[0x570 - 0x3FC];
    s32 targetCount;
    u32 counter;
    u8 pad578[0x5C0 - 0x578];
    u16 proximityTimer;
};
extern u32 gUnk_02000560;
void ScenePanel_DispatchTransitionCommands(void *);
void Vanargand_UpdatePartAnimation(void *, void *);
void Obj_StepPosition(void *);
void SceneActors_UpdateAnimation(void *, void *, void *);
void PositionedEffect_UpdateOscillatingCoordinates(void *, u32);
void ScreenPalette_ProjectPositionAt88(void *);
void PositionedEffect_InterpolateArcNodes(void *);
void PositionedEffect_UpdateVectors(void *);
void SceneActors_UpdateTargetMetrics(void *);
s32 Math_Atan2(s32, s32);
static inline u32 TestFlags(struct VanargandTargets *p, u32 mask)
{
    return p->flags & mask;
}
static inline u32 TestGlobal(u32 mask, u32 *flags)
{
    return *flags & mask;
}
void Vanargand_UpdateAnimationAndTargets(struct VanargandTargets *p)
{
    struct VanargandTargets *actor = p;
    struct VanargandTargets *owner = p;
    struct TargetSprite *sprite = &actor->sprite;
    s32 i;
    ScenePanel_DispatchTransitionCommands(actor);
    Vanargand_UpdatePartAnimation(actor, actor->part);
    Obj_StepPosition(sprite);
    if (!TestFlags(actor, 0x4000000))
        SceneActors_UpdateAnimation(actor, actor->animation, actor->drawable);
    actor->renderPosition.x = sprite->position.x;
    actor->renderPosition.y = sprite->position.y;
    {
        u32 offset;
        s16 z = sprite->position.z;
        offset = 0x210;
        {
            s16 *out = (s16 *)((u8 *)actor + offset);
            *out = z;
        }
    }
    if (!TestFlags(actor, 0x100000) && !TestGlobal(4, &gUnk_02000560))
        PositionedEffect_UpdateOscillatingCoordinates(actor, actor->counter);
    ScreenPalette_ProjectPositionAt88(actor->projection);
    PositionedEffect_InterpolateArcNodes(actor->arc);
    PositionedEffect_UpdateVectors(actor->animation);
    SceneActors_UpdateTargetMetrics(owner);
    i = 0;
    if (i < actor->targetCount) {
        struct TargetSprite *base = &owner->sprite;
        do {
            struct TargetVec *target = owner->targets[i];
            if (target) {
                s32 x = target->x - base->position.x;
                s32 z = target->z - base->position.z;
                owner->bearings[i] = Math_Atan2(x, z);
                owner->distances[i] = x * x + z * z;
            }
            i++;
        } while (i < actor->targetCount);
    }
    if ((u32)actor->distances[0] <= 0x3FFFF) {
        if (actor->player->state != 24)
            actor->proximityTimer++;
    } else {
        actor->proximityTimer = 0;
    }
}
