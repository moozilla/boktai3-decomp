#include "scene_actor_chase_shared.h"
#include "scene_actor_control_phase.h"
#include "engine.h"
#include "ram.h"
void SceneActorReaction_ApplyThreshold(struct SceneActorRegistryResetView *, s32);
void Skeleton_InitializeCategoryProfile(void *);
void Skeleton_InitVariantMotion(void *);
void Collider_SetAttackProperties(void *, u16, u16, u32, u32, u16);
void sub_08127590(void *, void *, u32, u32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern const s16 gUnk_086149C4[];

static inline s32 Skeleton_TruncateFixed6(s32 x)
{
    if (x >= 0)
        return x >> 6;
    return -((-x) >> 6);
}
static inline s32 Skeleton_TruncateFixed12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
static inline u16 Skeleton_RandomSample(void)
{
    const u16 *samples = (const u16 *)RANDOM_TABLE_ADDR;
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)((u32)samples + index);
}
static inline void Skeleton_SetPolarMotion(struct ContactPosition *v, s32 direction, s32 radius)
{
    v->x = Skeleton_TruncateFixed12(gUnk_086149C4[(direction + 64) & 255] * radius);
    v->y = 0;
    v->z = Skeleton_TruncateFixed12(gUnk_086149C4[direction & 255] * radius);
}

struct SkeletonAttackContext {
    u8 unknown0[0x678];
    struct ContactCollider attack;
};

static inline void Skeleton_AddPosition(struct ContactPosition *position,
                                        const struct ContactPosition *base,
                                        const struct ContactPosition *delta)
{
    position->x = delta->x + base->x;
    position->y = delta->y + base->y;
    position->z = delta->z + base->z;
}

static inline void Skeleton_SetCategory(struct SceneActorRegistryResetView *actor, u8 category)
{
    actor->category = category;
}

static inline void Skeleton_EmitBone(struct ContactPosition *position,
                                     struct ContactPosition *velocity, u32 sequence)
{
    sub_08127590(position, velocity, 0xD45E, 0x1DC3, sequence, 30, 2, 0, 316, 0, 0, 0, 0, 0, 0);
}

/* Emit two bone fragments and restore the threshold reaction. */
void sub_080BA268(struct SceneActorRegistryResetView *actor)
{
    struct ContactPosition headingMotion;
    struct ContactPosition velocity;
    struct ContactPosition position;
    s32 direction;
    struct SkeletonAttackContext *context = actor->context;

    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0x16);
        direction = Skeleton_TruncateFixed6((s32)((actor->body.direction + 0x10) & 0xFF)) << 6;
        Skeleton_SetPolarMotion(&headingMotion, direction, 20);
        direction = Skeleton_RandomSample();
        direction &= 255;
        Skeleton_SetPolarMotion(&velocity, direction, 10);
        velocity.y = 30;
        position = actor->body.position;
        position.y += 0x104;
        Skeleton_AddPosition(&position, &position, &velocity);
        Skeleton_EmitBone(&position, &velocity, 2);
        direction = Skeleton_RandomSample();
        direction &= 255;
        Skeleton_SetPolarMotion(&velocity, direction, 10);
        velocity.y = 30;
        position.y += 0xDC;
        Skeleton_AddPosition(&position, &position, &velocity);
        Skeleton_EmitBone(&position, &velocity, 3);
        Skeleton_SetCategory(actor, 5);
        Skeleton_InitializeCategoryProfile(actor);
        Skeleton_InitVariantMotion(actor);
        {
            u16 first = actor->attacks.first[3];
            u16 second = actor->attacks.second[3];
            u32 flags = actor->attacks.flags[3];
            u16 parameter = actor->attacks.parameter[3];
            Collider_SetAttackProperties(&context->attack, first, second, 0, flags, parameter);
        }
    }
    if (actor->field2B3)
        SceneActorChase_SetControl(actor, SceneActorReaction_ApplyThreshold, 0x3E);
}
