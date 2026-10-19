#define SceneActorReaction_WaitHiddenTarget sub_080EB168
#include "global.h"
#include "scene_actor_reaction.h"
s32 sub_080EC0E8(struct SceneActorRegistryResetView *, s32);
void sub_080E990C(void *);
static inline u8 TakeReset(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
void sub_080EB168(struct SceneActorRegistryResetView *actor)
{
    s32 result;
    if (TakeReset(actor)) {
        struct ContactCollider *collider = &actor->colliderA;
        u32 mask;
        collider->flags = (4 | collider->flags) & 0xFFF7;
        mask = 0x800;
        actor->flags2C0 = mask | actor->flags2C0;
        mask = 1;
        if (!actor->drawableKind)
            *(u32 *)actor->drawable |= mask;
        else {
            struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->drawable + 0x20);
            tile->flags |= mask;
        }
    }
    result = sub_080EC0E8(actor, 1);
    if (!result) {
        if ((s32)actor->counter2AC > 60) {
            u32 phase = 2;
            u32 behavior = 5;
            actor->state2A8 = result;
            actor->phase2A9 = phase;
            actor->behavior2AA = behavior;
            actor->counter2AC = result;
            actor->field2B1 = 1;
        } else {
            actor->counter2AC++;
        }
    } else {
        u32 behavior = 2;
        u8 *field = &actor->state2A8;
        u32 zero = 0;
        *field = zero;
        actor->phase2A9 = zero;
        actor->behavior2AA = behavior;
        actor->counter2AC = zero;
        actor->field2B1 = 1;
        {
            void *callback = sub_080E990C;
            u32 kind = 41;
            actor->field2B2 = 1;
            actor->field2B0 = zero;
            actor->field2B5 = kind;
            actor->target = callback;
            actor->field2C8 = zero;
            {
                u32 *flags = &actor->flags114;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->word104 = zero;
        }
    }
}
