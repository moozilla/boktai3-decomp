#include "global.h"
#include "scene_actor_reaction.h"
/* After 60 frames, return to the group route point and hand control to the entry target. */
struct SceneActorReactionContext {
    u8 unknown0[0x6DC];
    u32 flags6DC;
};
struct SceneActorReactionRoutePoint {
    u8 unknown0[0x1C];
    s16 x;
    u8 unknown1E[2];
    s16 z;
};
void SceneActor_SetPositionXZ(void *, s32, s32);
static inline u8 IsActive(u32 value)
{
    if (value)
        return 1;
    return 0;
}
static inline u32 Reaction_MaskFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.flags2D4 & mask;
}
static inline void Reaction_SetControl(struct ChaseActor *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
void SceneActorReaction_ReturnToRouteEntry(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorReactionContext *context = actor->base.context;
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 *flags;
        if (!Reaction_MaskFlags(actor, 128)) {
            u32 mask = 0x2000;
            u32 *address = &context->flags6DC;
            u32 value = *address & mask;
            flags = address;
            if (!IsActive(value))
                goto twelve;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        goto checked;
    twelve:
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 12);
        {
            u32 mask = 0x2000;
            *flags |= mask;
        }
    checked:;
    }
    if (frame > 60) {
        u16 blocked;
        actor->base.field2B0 = 1;
        blocked = Reaction_MaskFlags(actor, 128);
        if (blocked) {
            void *target;
            SceneActor_SetPositionXZ(
                &actor->base.body.id,
                (*(struct SceneActorReactionRoutePoint **)((u8 *)group + 0x1FC))->x,
                (*(struct SceneActorReactionRoutePoint **)((u8 *)group + 0x1FC))->z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            target = group->gate.control;
            Reaction_SetControl(actor, 26, target);
            {
                u32 behavior = 12;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = behavior;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = 1;
            }
            {
                u16 *f = &group->gate.flags;
                u16 four = 4;
                *f |= four;
            }
            actor->base.field2C2 = 32;
        } else {
            void *target;
            SceneActor_SetPositionXZ(
                &actor->base.body.id,
                (*(struct SceneActorReactionRoutePoint **)((u8 *)group + 0x1FC))->x,
                (*(struct SceneActorReactionRoutePoint **)((u8 *)group + 0x1FC))->z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            target = actor->base.entryTarget228;
            Reaction_SetControl(actor, 1, target);
            {
                u32 behavior = 7;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = behavior;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = 1;
            }
            {
                u32 mask = 16;
                actor->base.flags2CC |= mask;
            }
            actor->base.field2C2 = 32;
        }
    }
}
