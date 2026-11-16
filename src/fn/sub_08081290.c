#include "scene_actor_terrain_reaction.h"
#include "scene_actor_reaction.h"
void SceneActorAttachment_ReplaceResponseSlot(struct ChaseActor *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void SceneActorReaction_BeginHiddenDefeat(struct ChaseActor *);
static inline u8 IsControlled(struct ChaseActor *a)
{
    if (a->base.object == (void *)1)
        return 1;
    return 0;
}

static inline void ClearRoute(struct ChaseActor *a)
{
    struct SceneActorRegistryControl *route = &a->group.common.control;
    u32 *flags = &a->base.behaviorFlags;
    u32 mask = ~0x141C0;
    *flags &= mask;
    route->counter = 0;
    route->field14 = 0;
    route->timer = 0;
}

static inline void Show(struct ChaseActor *a)
{
    if (!a->base.drawableKind) {
        u32 *flags = a->base.drawable;
        u32 mask = ~1;
        *flags &= mask;
    } else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)a->base.drawable + 0x20);
        u32 mask = ~1;
        tile->flags &= mask;
    }
}

void SceneActorReaction_SinkAndRemove(struct ChaseActor *actor, s32 frame)
{
    u32 state;

    if (SceneActorReaction_TakePrepared(actor)) {
        struct ContactCollider *collider = &actor->base.colliderA;
        u32 category = actor->base.category;
        if (IsControlled(actor))
            ClearRoute(actor);
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        if (category != 2 && category != 22) {
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, (u8)category);
        }
        {
            u16 *destination = &collider->flags;
            u32 flags = *destination;
            u32 mask = 4;
            mask |= flags;
            *destination = mask;
        }
        Show(actor);
        {
            u32 zero;
            u8 *first;
            state = 3;
            first = &actor->base.state2A8;
            zero = 0;
            *first = state;
            actor->base.phase2A9 = zero;
            actor->base.behavior2AA = zero;
            actor->base.counter2AC = zero;
            actor->base.field2B1 = 1;
        }
        actor->base.attachmentTimer = 0;
        actor->base.restart = 0;
        collider->responseValue = 0;
        actor->base.hitLockTimer = 0;
        {
            u32 mask = 0x20000;
            u32 *flags = &actor->base.statusFlags;
            *flags |= mask;
        }
    }
    if (frame > 6) {
        actor->base.body.position.y -= (u32)(frame - 6) * 30;
        if (!actor->base.drawableKind)
            *(struct ContactPosition *)((u8 *)actor->base.drawable + 0x1C) =
                actor->base.body.position;
        else
            *(struct ContactPosition *)((u8 *)actor->base.drawable + 0x40) =
                actor->base.body.position;
        if (frame > 8) {
            u16 zeroWord = 0;
            u32 zeroByte = 0;
            {
                s16 health = zeroWord;
                actor->base.health = health;
            }
            {
                u32 mask = 8;
                u16 *field = &actor->base.motionFlags;
                mask |= *field;
                *field = mask;
            }
            {
                u32 mask = 0x400;
                u32 *flags = &actor->base.behaviorFlags;
                *flags |= mask;
            }
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
            SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
            SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
            SceneActorEffects_AdvanceSteps(actor, 4);
            SceneActorTerrain_InstallControl(actor, SceneActorReaction_BeginHiddenDefeat, 23);
            if (!actor->base.drawableKind)
                *(u32 *)actor->base.drawable |= 1;
            else {
                struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->base.drawable + 0x20);
                tile->flags |= 1;
            }
        }
    }
}
