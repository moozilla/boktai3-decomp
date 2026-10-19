#include "global.h"
#include "scene_actor_reaction.h"
void Ivy_UpdateActiveState_080E98B4(void);
static inline void clearFlag(u32 *flags)
{
    u32 mask = ~1;
    *flags &= mask;
}
static inline void clearColliderFlag(u16 *flags)
{
    u32 mask = ~0x800;
    mask &= *flags;
    *flags = mask;
}
static inline void showActor(struct SceneActorRegistryResetView *actor)
{
    if (actor->drawableKind == 0)
        clearFlag(actor->drawable);
    else {
        u8 *drawable = (u8 *)actor->drawable + 0x20;
        u32 mask = ~1;
        *(u32 *)(drawable + 8) &= mask;
    }
}
void SceneActor_BeginReactionAnimation(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        u8 *drawable = actor->drawable;
        u32 zero = 0;
        drawable[7] = 2;
        ((struct SceneActorRegistryActor *)actor)->field178 = zero;
        showActor(actor);
        actor->colliderA.flags = (actor->colliderA.flags & 0xFFFB) | 8;
        clearColliderFlag(&actor->flags2C0);
        SceneActorAnimation_SelectDirectionalFlags((void *)actor, 15);
        Sound_PlaySong(0x2A0);
    }
    if (actor->field2B3) {
        void (*callback)(void) = Ivy_UpdateActiveState_080E98B4;
        u32 kind = 0x22;
        u32 zero;
        u8 *prepared = &actor->field2B2;
        u32 *flags;
        s32 mask;
        zero = 0;
        *prepared = 1;
        actor->field2B0 = zero;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = zero;
        flags = &actor->flags114;
        mask = -2;
        *flags = mask & *flags;
        actor->word104 = zero;
    }
}
