#include "scene_actor_registry.h"
#include "scene_actor_reaction.h"
#include "engine.h"
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
static inline u8 IsControlled(struct ChaseActor *a)
{
    if (a->base.object == (void *)1)
        return 1;
    return 0;
}
static inline void ClearRoute(struct ChaseActor *a, u8 *group)
{
    struct SceneActorRegistryControl *route = (void *)(group + 0x4C);
    u32 *flags = &a->base.behaviorFlags;
    u32 mask = ~0x141C0;
    *flags &= mask;
    route->counter = 0;
    route->field14 = 0;
    route->timer = 0;
}
static inline void SetState(struct ChaseActor *actor, u8 s, u8 p, u8 b)
{
    actor->base.state2A8 = s;
    actor->base.phase2A9 = p;
    actor->base.behavior2AA = b;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
static inline void SetControl(struct ChaseActor *actor, void *cb, u8 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = cb;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}
void SceneActorReaction_PlayStaggerBurst(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 category = actor->base.category;
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
        if (!actor->base.drawableKind) {
            u32 *flags = actor->base.drawable;
            u32 mask = ~1;
            *flags &= mask;
        } else {
            struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->base.drawable + 0x20);
            u32 mask = ~1;
            tile->flags &= mask;
        }
        // The state is held in a variable for the store and then reset, so the
        // random mask below takes its own 3 from the bitfield store (r4 in the ROM).
        state = 3;
        actor->base.state2A8 = 3;
        state = 0;
        actor->base.phase2A9 = 0;
        actor->base.behavior2AA = 0;
        actor->base.counter2AC = 0;
        actor->base.field2B1 = 1;
        actor->base.restart = 0;
        {
            u16 *flags = &actor->base.motionFlags;
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
            SparkParticle_Spawn(&actor->base.body.position, mode, &range.position,
                                actor->base.body.direction + 128, 10, 6, 2, 2);
        }
    }
    if (!actor->base.drawableKind) {
        u8 *sequence = (u8 *)actor->base.drawable + 0x48;
        *(u16 *)(sequence + 0xE) = 0;
    } else
        ((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->state2C = 0;
    if (frame > 15) {
        u32 mask = 2;
        u16 masked = actor->base.motionFlags;
        masked &= mask;
        if (masked) {
            SetState(actor, 3, 0, 0);
            SetControl(actor, group->contactControl, 7);
        } else {
            SetControl(actor, actor->base.onReaction, 18);
            SetState(actor, 3, 0, 0);
        }
    }
}
