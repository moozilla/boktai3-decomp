#include "scene_actor_registry.h"
#include "scene_actor_reaction.h"
#include "engine.h"
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
static inline u8 IsControlled(struct SceneActorRegistryResetView *a)
{
    if (a->object == (void *)1)
        return 1;
    return 0;
}
static inline void ClearRoute(struct SceneActorRegistryResetView *a, u8 *group)
{
    struct SceneActorRegistryControl *route = (void *)(group + 0x4C);
    u32 *flags = &a->flags2CC;
    u32 mask = ~0x141C0;
    *flags &= mask;
    route->counter = 0;
    route->field14 = 0;
    route->timer = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 s, u8 p, u8 b)
{
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *cb, u8 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = cb;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
void SceneActorReaction_PlayStaggerBurst(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 category = actor->category;
        u32 state;
        /* Named components are initialized; aggregate padding is never read as a scalar. */
        union {
            struct ContactPackedVector packed;
            struct ContactPosition position;
        } range;
        if (IsControlled(actor))
            ClearRoute(actor, (u8 *)group);
        if (category == 22)
            SceneActorAnimation_SelectProfileEntry((void *)actor, 3);
        else
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        if (!actor->drawableKind) {
            u32 *flags = actor->drawable;
            u32 mask = ~1;
            *flags &= mask;
        } else {
            struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->drawable + 0x20);
            u32 mask = ~1;
            tile->flags &= mask;
        }
        // The state is held in a variable for the store and then reset, so the
        // random mask below takes its own 3 from the bitfield store (r4 in the ROM).
        state = 3;
        actor->state2A8 = 3;
        state = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
        actor->field2B3 = 0;
        {
            u16 *flags = &actor->flags2D4;
            u32 mask = ~0x20;
            mask &= *flags;
            *flags = mask;
        }
        SceneActorPlacement_PlayCategoryResponseSound((void *)actor);
        Sound_PlaySong(298);
        range.packed.x = 60;
        range.packed.y = 60;
        range.packed.z = 40;
        {
            u32 mode;
            gRandomIndex = (gRandomIndex + 1) & 1023;
            mode = *(u16 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1)) & 3;
            mode++;
            SparkParticle_Spawn(&actor->body.position, mode, &range.position,
                                actor->body.direction + 128, 10, 6, 2, 2);
        }
    }
    if (!actor->drawableKind) {
        u8 *sequence = (u8 *)actor->drawable + 0x48;
        *(u16 *)(sequence + 0xE) = 0;
    } else
        ((struct SceneActorRegistryTileDrawable *)actor->drawable)->state2C = 0;
    if (frame > 15) {
        u32 mask = 2;
        u16 masked = actor->flags2D4;
        masked &= mask;
        if (masked) {
            SetState(actor, 3, 0, 0);
            SetControl(actor, group->contactControl, 7);
        } else {
            SetControl(actor, actor->reactionTarget230, 18);
            SetState(actor, 3, 0, 0);
        }
    }
}
