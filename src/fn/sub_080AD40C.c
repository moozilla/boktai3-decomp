#include "global.h"
#include "scene_actor_reaction.h"
struct SceneActorReactionContext {
    u8 unknown0[0x6D4];
    u16 flags;
};
struct SceneActorReactionRoutePoint {
    u8 unknown0[0x1C];
    s16 x;
    u8 unknown1E[2];
    s16 z;
};
void SceneActor_SetPositionXZ(void *, s32, s32);
static inline u32 Reaction_MaskFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline void Reaction_SetControl(struct SceneActorRegistryResetView *actor, u32 kind,
                                       void *control)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
void SceneActorReaction_ResumeSavedTarget(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct SceneActorReactionContext *context = actor->context;
    struct SceneActorRegistryControlGroup *group = &actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->field2B3 = 1;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 12);
        {
            u16 *flags = &context->flags;
            s32 mask = ~12;
            *flags &= mask;
        }
    }
    if (frame > 60 && actor->field2B3) {
        u16 blocked;
        actor->field2B0 = 1;
        blocked = Reaction_MaskFlags(actor, 128);
        if (!blocked) {
            void *target;
            SceneActor_SetPositionXZ(
                &actor->body.id, (*(struct SceneActorReactionRoutePoint **)((u8 *)group + 0x1FC))->x,
                (*(struct SceneActorReactionRoutePoint **)((u8 *)group + 0x1FC))->z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            target = actor->entryTarget228;
            Reaction_SetControl(actor, 1, target);
            {
                u32 behavior = 7;
                actor->state2A8 = 0;
                actor->phase2A9 = 0;
                actor->behavior2AA = behavior;
                actor->counter2AC = 0;
                actor->field2B1 = 1;
            }
            {
                u32 mask = 16;
                actor->flags2CC |= mask;
            }
        }
    }
}
