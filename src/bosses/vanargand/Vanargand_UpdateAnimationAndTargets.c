#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
#include "scene/actor/scene_actor_components.h"
#include "player/player.h"

// confidence=low: advances presentation and caches bearings and planar target distances.
struct VanargandTargets {
    u8 pad0[0x48];
    struct SceneActorBody sprite;
    u8 pad5C[0x9C - 0x48 - sizeof(struct SceneActorBody)];
    u32 flags;
    u8 padA0[0xC8 - 0xA0];
    s32 distances[1];
    u8 bearings[1];
    u8 padCD[0x10C - 0xCD];
    struct Player *player;
    struct SceneVec *targets[1];
    u8 pad114[0x174 - 0x114];
    u8 projection[0x18C - 0x174];
    u8 animation[0x60];
    u8 drawable[0x20C - 0x1EC];
    struct ContactPosition renderPosition;
    u8 pad214[0x2D0 - 0x214];
    u8 arc[0x3FC - 0x2D0];
    u8 part[0x570 - 0x3FC];
    s32 targetCount;
    u32 counter;
    u8 pad578[0x5C0 - 0x578];
    u16 proximityTimer;
};
extern u32 gUnk_02000560;
void VanargandPair_DispatchTransitionCommands(void *);
void Vanargand_UpdatePartAnimation(void *, void *);
void Obj_StepPosition(void *);
void SceneActors_UpdateAnimation(void *, void *, void *);
void PositionedEffect_UpdateOscillatingCoordinates(void *, u32);
void ScreenPalette_ProjectPositionAt88(void *);
void PositionedEffect_InterpolateArcNodes(void *);
void PositionedEffect_UpdateVectors(void *);
void SceneActors_UpdateTargetMetrics(void *);
s32 Math_Atan2(s32, s32);
static inline u32 TestGlobal(u32 mask, u32 *flags)
{
    return *flags & mask;
}
void Vanargand_UpdateAnimationAndTargets(struct VanargandTargets *p)
{
    struct VanargandTargets *owner = p;
    struct SceneActorBody *sprite = &p->sprite;
    s32 i;
    VanargandPair_DispatchTransitionCommands(p);
    Vanargand_UpdatePartAnimation(p, p->part);
    Obj_StepPosition(sprite);
    if (!VanargandPairAttack_TestFlags((struct VanargandPairAttack *)p, 0x4000000))
        SceneActors_UpdateAnimation(p, p->animation, p->drawable);
    p->renderPosition.x = sprite->position.x;
    p->renderPosition.y = sprite->position.y;
    p->renderPosition.z = sprite->position.z;
    if (!VanargandPairAttack_TestFlags((struct VanargandPairAttack *)p, 0x100000) &&
        !TestGlobal(4, &gUnk_02000560))
        PositionedEffect_UpdateOscillatingCoordinates(p, p->counter);
    ScreenPalette_ProjectPositionAt88(p->projection);
    PositionedEffect_InterpolateArcNodes(p->arc);
    PositionedEffect_UpdateVectors(p->animation);
    SceneActors_UpdateTargetMetrics(owner);
    i = 0;
    if (i < p->targetCount) {
        struct SceneActorBody *base = &owner->sprite;
        do {
            struct SceneVec *target = owner->targets[i];
            if (target) {
                s32 x = target->x - base->position.x;
                s32 z = target->z - base->position.z;
                owner->bearings[i] = Math_Atan2(x, z);
                owner->distances[i] = x * x + z * z;
            }
            i++;
        } while (i < p->targetCount);
    }
    if ((u32)p->distances[0] <= 0x3FFFF) {
        if (p->player->tetherMode != 24)
            p->proximityTimer++;
    } else {
        p->proximityTimer = 0;
    }
}
