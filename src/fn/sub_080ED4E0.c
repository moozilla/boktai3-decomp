#include "global.h"
// confidence=low: attempts a grab on the active animation frames and positions its target.
#define Ivy_UpdateGrabAttempt sub_080ED4E0
#ifdef NONMATCHING
// fuzzy: 80.2%
// fix.py best (sweep after the inlined-insn fix)
// Full grab and fallback flow; animation and reset-field lifetimes still differ.
#include "scene_actor_chase.h"
#include "scene_actor_reaction.h"
#include "scene_vec.h"
struct IvyTarget {
    u8 unknown0[8];
    u8 kind;
    u8 unknown9[0x33];
    struct SceneActorRegistryResetView *actor;
};
struct IvyContext {
    u8 unknown0[0x62C];
    u32 flags;
};
struct IvyOffset {
    u16 x, y, z, pad;
};
extern const struct IvyOffset gUnk_08606384[2][5];
void *Ivy_FindAdjacentTarget(struct SceneActorRegistryResetView *, s32);
void Ivy_UpdateHold(void);
void SceneActorReaction_FinishAnimation17(void);
s32 Player_BeginPositionRequest(void *, struct SceneVec *, struct SceneVec *, s32, s32, s32, s32,
                                s32, u32);
void sub_0807D2E0(void *, u32, struct ContactPosition *, u32, u32, u32);
static inline void Ivy_Facing(struct SceneActorRegistryResetView *actor, u8 *quadrant, u8 *flip)
{
    s32 side = ((actor->direction + 0x60) >> 6) & 3;
    if (side > 1) {
        *flip = 1;
        *quadrant = 3 - side;
    } else {
        *flip = 0;
        *quadrant = side;
    }
}
static inline void Ivy_ClearHalf(u16 *field, s32 mask)
{
    *field &= mask;
}
static inline void Ivy_SetState(struct SceneActorRegistryResetView *actor, u32 phase, u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void Ivy_EnableContact(struct SceneActorRegistryResetView *actor)
{
    actor->colliderA.flags = (actor->colliderA.flags | 4) & 0xFFF7;
    {
        u32 mask = 0x800;
        actor->flags2C0 = mask | actor->flags2C0;
    }
}
static inline void Ivy_DisableContact(struct SceneActorRegistryResetView *actor)
{
    struct ContactCollider *collider = &actor->colliderA;
    collider->flags = (collider->flags & 0xFFFB) | 8;
    Ivy_ClearHalf(&actor->flags2C0, ~0x800);
}
inline static void Ivy_Facing_080ED4E0(struct SceneActorRegistryResetView *actor, u8 *quadrant,
                                       u8 *flip)
{
    s16 side = ((actor->direction + 0x60) >> 6) & 3;
    if (side > 1) {
        *flip = 1;
        *quadrant = 3 - side;
    } else {
        *flip = 0;
        *quadrant = side;
    }
}

inline static void Ivy_EnableContact_080ED4E0(struct SceneActorRegistryResetView *actor)
{
    do {
        actor->colliderA.flags = (actor->colliderA.flags | 4) & 0xFFF7;
    } while (0);
    {
        u32 mask = 0x800;
        actor->flags2C0 = mask | actor->flags2C0;
    }
}

void Ivy_UpdateGrabAttempt(struct SceneActorRegistryResetView *actor)
{
    struct DisplaySequenceState *sequence = (void *)(((u8 *)actor->drawable) + 0x48);
    u8 quadrant;
    u8 flip;
    u16 index;
    struct ContactPosition position;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
    if ((sequence->index == 3) && (sequence->elapsed == 0))
        Sound_PlaySong(477);
    index = sequence->index;
    if ((index == 3) && (((s32)sequence->elapsed) > 6)) {
        Ivy_EnableContact_080ED4E0(actor);
        if (Ivy_FindAdjacentTarget(actor, 0)) {
            struct IvyTarget *target;
            struct IvyContext *context;
            Chase_SetCtl(actor, Ivy_UpdateHold, 0x25);
            Ivy_SetState(actor, 2, 1);
            Ivy_Facing_080ED4E0(actor, &quadrant, &flip);
            position.x = actor->position.x + gUnk_08606384[flip][sequence->index].x;
            position.y = actor->position.y + gUnk_08606384[flip][sequence->index].y;
            position.z = actor->position.z + gUnk_08606384[flip][sequence->index].z;
            target = (void *)actor->fieldDC;
            if (target->kind <= 3) {
                Player_BeginPositionRequest(((void **)gUnk_02000580)[target->kind],
                                            (void *)(&position), 0, 0, flip, 0, 0, 0, 0);
            } else if (target->kind == 4) {
                struct SceneActorRegistryResetView *victim = target->actor;
                *((u16 *)(((u8 *)victim) + 0x178)) = 0;
                SceneActorTerrain_ClearCollisionTimers((void *)victim);
                Ivy_ClearHalf(&victim->flags2D4, ~0x20);
                sub_0807D2E0(victim, 0, &position, 0, flip, 0);
            }
            context = actor->context;
            {
                u32 mask = 2;
                context->flags |= mask;
            }
            return;
        }
    } else if (((u16)index) <= 1) {
        Ivy_DisableContact(actor);
    } else {
        Ivy_EnableContact_080ED4E0(actor);
    }
    if (actor->field2B3) {
        Ivy_DisableContact(actor);
        Chase_SetCtl(actor, SceneActorReaction_FinishAnimation17, 0x27);
        Ivy_SetState(actor, 2, 3);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080ED4E0);
#endif
