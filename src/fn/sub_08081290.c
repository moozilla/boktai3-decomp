#include "scene_actor_terrain_reaction.h"
#include "scene_actor_reaction.h"
void SceneActorAttachment_ReplaceResponseSlot(struct SceneActorRegistryResetView *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void SceneActorReaction_BeginHiddenDefeat(struct SceneActorRegistryResetView *);
static inline u8 IsControlled(struct SceneActorRegistryResetView *a)
{
    if (a->object == (void *)1)
        return 1;
    return 0;
}

static inline void ClearRoute(struct SceneActorRegistryResetView *a)
{
    struct SceneActorRegistryControl *route = &a->group.common.control;
    u32 *flags = &a->flags2CC;
    u32 mask = ~0x141C0;
    *flags &= mask;
    route->counter = 0;
    route->field14 = 0;
    route->timer = 0;
}

static inline void Show(struct SceneActorRegistryResetView *a)
{
    if (!a->drawableKind) {
        u32 *flags = a->drawable;
        u32 mask = ~1;
        *flags &= mask;
    } else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)a->drawable + 0x20);
        u32 mask = ~1;
        tile->flags &= mask;
    }
}

void SceneActorReaction_SinkAndRemove(struct SceneActorRegistryResetView *actor, s32 frame)
{
    u32 state;

    if (SceneActorReaction_TakePrepared(actor)) {
        struct ContactCollider *collider = &actor->colliderA;
        u32 category = actor->category;
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
            first = &actor->state2A8;
            zero = 0;
            *first = state;
            actor->phase2A9 = zero;
            actor->behavior2AA = zero;
            actor->counter2AC = zero;
            actor->field2B1 = 1;
        }
        actor->fieldAA = 0;
        actor->field2B3 = 0;
        collider->responseValue = 0;
        actor->field2B8 = 0;
        {
            u32 mask = 0x20000;
            u32 *flags = &actor->flags2D0;
            *flags |= mask;
        }
    }
    if (frame > 6) {
        actor->body.position.y -= (u32)(frame - 6) * 30;
        if (!actor->drawableKind)
            *(struct ContactPosition *)((u8 *)actor->drawable + 0x1C) = actor->body.position;
        else
            *(struct ContactPosition *)((u8 *)actor->drawable + 0x40) = actor->body.position;
        if (frame > 8) {
            u16 zeroWord = 0;
            u32 zeroByte = 0;
            {
                s16 health = zeroWord;
                actor->health = health;
            }
            {
                u32 mask = 8;
                u16 *field = &actor->flags2D4;
                mask |= *field;
                *field = mask;
            }
            {
                u32 mask = 0x400;
                u32 *flags = &actor->flags2CC;
                *flags |= mask;
            }
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
            SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
            SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
            SceneActorEffects_AdvanceSteps(actor, 4);
            SceneActorTerrain_InstallControl(actor, SceneActorReaction_BeginHiddenDefeat, 23);
            if (!actor->drawableKind)
                *(u32 *)actor->drawable |= 1;
            else {
                struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->drawable + 0x20);
                tile->flags |= 1;
            }
        }
    }
}
