#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorControl_OnIdle(u8 *);
static inline u8 IsUnavailable(struct SceneActorRegistryActor *actor, u16 **saved, u32 excluded)
{
    u16 *flags = &actor->flags2D4;
    u32 value = *flags;
    u32 mask = 0x40;
    value &= mask;
    *saved = flags;
    if (value)
        goto one;
    {
        u32 value = actor->flags2D0;
        mask = excluded;
        value &= mask;
        if (value)
            goto one;
    }
    if (actor->health > 0)
        goto zero;
one:
    mask = 1;
    return mask;
zero:
    mask = 0;
    return mask;
}

void SceneActorRegistry_ResetEligibleCategoryState(void)
{
    struct SceneActorRegistryNode *node;
    struct SceneActorRegistryActor *actor;
    SceneActorRegistry_GetManager();
    node = gSceneActorRegistryNode;
    actor = node->actor;
    if (actor) {
        u32 excluded = 0x20000;
        u32 reject = 0xa007;
        const void *callback = (const void *)SceneActorControl_OnIdle;
        u32 substate = 16;
        u32 one = 1;
        u32 zero = 0;
        do {
            u16 *flags;
            {
                u32 unavailable = IsUnavailable(actor, &flags, excluded);
                if (!unavailable) {
                    u32 category = actor->category;
                    if (category == 4 || category == 1 || category == 3) {
                        u32 selected = *flags & reject;
                        if (!selected) {
                            actor->field2B2 = one;
                            actor->field2B0 = selected;
                            actor->unknown2B5[0] = substate;
                            actor->control = callback;
                            actor->field2C8 = selected;
                            {
                                u32 *flags = &actor->flags114;
                                u32 mask = ~1;
                                u32 value = *flags;
                                value &= mask;
                                *flags = value;
                            }
                            actor->field104 = selected;
                            actor->state2A8 = zero;
                            actor->phase2A9 = one;
                            actor->behavior2AA = zero;
                            actor->counter2AC = selected;
                            actor->field2B1 = one;
                        }
                    }
                }
            }
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
}
