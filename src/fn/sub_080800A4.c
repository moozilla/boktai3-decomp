#include "global.h"
// confidence=low: restores route control or enters defeat when the contact response completes.
#define SceneActorReaction_UpdateContactRouteRecovery sub_080800A4
#ifdef NONMATCHING
// fuzzy: 64.3%
// Full state flow recovered; field lifetimes and route/reset allocation still differ.
#include "scene_actor_chase.h"
#include "engine.h"

struct ReactionRoutePacket {
    u8 unknown0[8];
    u8 state, phase, behavior, unknownB;
    u32 delay;
    void *control;
};

static inline void Reaction_ClearWord(u32 *field, u32 bits)
{
    u32 mask = ~bits;
    *field &= mask;
}
static inline void Reaction_ClearHalf(struct ChaseFlags40 *field, u32 bits)
{
    s32 mask = ~bits;
    field->bits &= mask;
}
static inline void Reaction_SetActorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2CC = mask | actor->flags2CC;
}
static inline u32 *Reaction_ClearRoute(struct SceneActorRegistryResetView *actor, u8 *group)
{
    struct SceneActorRegistryControl *control = (void *)(group + 0x4C);
    u32 *flags = &actor->flags2CC;
    u32 mask = ~0x141C0;
    *flags &= mask;
    control->counter = 0;
    *(u16 *)((u8 *)control + 0x14) = 0;
    control->timer = 0;
    return flags;
}
static inline void Reaction_Show(struct SceneActorRegistryResetView *actor)
{
    if (!actor->drawableKind) {
        u32 *flags = actor->drawable;
        s32 mask = ~1;
        *flags = mask & *flags;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        s32 mask = ~1;
        tile->flags = mask & tile->flags;
    }
}
static inline void Reaction_SetDrawMode(struct SceneActorRegistryResetView *actor, u32 mode)
{
    if (!actor->drawableKind)
        *((u8 *)actor->drawable + 7) = mode;
    else
        *((u8 *)actor->drawable + 0x3A) = mode;
}
void SceneActorReaction_UpdateContactRouteRecovery(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    struct ContactCollider *collider = &actor->colliderA;
    u8 prepared = SceneActorReaction_TakePrepared(actor);
    u32 *motion = &actor->flags2D0;
    u32 *controlFlags;
    if (prepared) {
        controlFlags = Reaction_ClearRoute(actor, group);
        SceneActorAnimation_SelectDirectionFlagsVariant((void *)actor, 3, 1);
        collider->flags |= 4;
        collider->responseValue = 0;
        Reaction_Show(actor);
        actor->fieldAA = 0;
        Reaction_SetDrawMode(actor, 2);
        ActorShadow_Enable((void *)((u8 *)actor + 0x320));
        motion = &actor->flags2D0;
        Reaction_ClearWord(motion, 0x80000);
        Reaction_ClearHalf((void *)&actor->flags2C0, 0x20);
        Reaction_ClearWord(controlFlags, 0x400000);
    }
    if (*motion & 0x10) {
        if (actor->health <= 1) {
            actor->health = 0;
            actor->flags2D4 |= 8;
            Chase_SetCtl(actor, *(void **)((u8 *)actor + 0x234), 0x11);
            Chase_SetState(actor, 3, 0, 0);
        } else {
            struct ContactPosition *destination;
            Reaction_SetActorFlags(actor, 0x400000);
            Reaction_ClearWord(&actor->flags2D0, 4);
            Reaction_ClearHalf((void *)&collider->flags, 4);
            destination = *(struct ContactPosition **)(group + 0x200);
            Chase_SnapToRoute(destination, group + 0x74);
            if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->body.position,
                                *(struct ContactPosition **)(group + 0x200))) {
                Chase_SetCtl(actor, SceneActorControl_OnIdle, 0);
                Chase_SetState(actor, 3, 0, 0);
            } else {
                struct ReactionRoutePacket *packet = (void *)(group + 0x8C);
                (*(struct ContactPosition **)(group + 0x200))->pad = 1;
                Chase_SetState(actor, 3, 1, 0);
                Chase_SetCtl(actor, SceneActorReaction_RestoreAfterDelay, 14);
                Reaction_SetActorFlags(actor, 0x10);
                actor->field2C2 = 0x40;
                packet->delay = 30;
                packet->control = *(void **)((u8 *)actor + 0x228);
                packet->state = 0;
                packet->phase = 0;
                packet->behavior = 7;
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080800A4);
#endif
