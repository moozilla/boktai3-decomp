#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorControl_OnIdle(u8 *);
static inline u8 IsUnavailable(struct ChaseActor *actor, u16 **saved, u32 excluded)
{
    u16 *flags = &actor->base.motionFlags;
    u32 value = *flags;
    u32 mask = 0x40;
    value &= mask;
    *saved = flags;
    if (value)
        goto one;
    {
        u32 value = actor->base.statusFlags;
        mask = excluded;
        value &= mask;
        if (value)
            goto one;
    }
    if (actor->base.health > 0)
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
    struct ChaseActor *actor;
    SceneActorRegistry_GetManager();
    node = gSceneActorRegistryNode;
    actor = node->actor;
    if (actor) {
        u32 excluded = 0x20000;
        u32 reject = 0xa007;
        const void *callback = (const void *)SceneActorControl_OnIdle;
        u32 substate = 16;
        do {
            u16 *flags;
            {
                u32 unavailable = IsUnavailable(actor, &flags, excluded);
                if (!unavailable) {
                    u32 category = actor->base.category;
                    if (category == 4 || category == 1 || category == 3) {
                        u32 selected = *flags & reject;
                        if (!selected) {
                            actor->base.field2B2 = 1;
                            actor->base.stepDone = selected;
                            actor->base.controlKind = substate;
                            actor->base.target = callback;
                            actor->base.field2C8 = selected;
                            {
                                u32 *flags = &actor->base.pendingControlFlags;
                                u32 mask = ~1;
                                u32 value = *flags;
                                value &= mask;
                                *flags = value;
                            }
                            actor->base.pendingControl = selected;
                            actor->base.state2A8 = 0;
                            actor->base.phase2A9 = 1;
                            actor->base.behavior2AA = 0;
                            actor->base.counter2AC = selected;
                            actor->base.field2B1 = 1;
                        }
                    }
                }
            }
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
}
