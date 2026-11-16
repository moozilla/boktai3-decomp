#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
static inline u8 IsUnavailable(struct ChaseActor *actor, u32 disabled, u32 excluded)
{
    if ((actor->base.motionFlags & disabled) || (actor->base.statusFlags & excluded) ||
        actor->base.health <= 0)
        return 1;
    return 0;
}

/* Reset the observed control/state fields for eligible categories4,1,3.
 * Behavior3 and substate5 remain numeric: their game roles are unconfirmed. */
void SceneActorRegistry_ResetCategoryControl(void)
{
    struct SceneActorRegistryNode *node;
    struct ChaseActor *actor;
    SceneActorRegistry_GetManager();
    node = gSceneActorRegistryNode;
    actor = node->actor;
    if (actor) {
        u32 disabled = 0x40;
        u32 excluded = 0x20000;
        u32 five = 5;
        u32 zero = 0;
        u32 one = 1;
        u32 three = 3;
        u32 status = 0x1000;
        do {
            u32 unavailable = IsUnavailable(actor, disabled, excluded);
            if (!unavailable) {
                u32 category = actor->base.category;
                if (category == 4 || category == 1 || category == 3) {
                    const void *control = actor->group.routePhaseCallback;
                    actor->base.field2B2 = one;
                    actor->base.stepDone = zero;
                    actor->base.controlKind = five;
                    actor->base.target = control;
                    actor->base.field2C8 = zero;
                    {
                        u32 *flags = &actor->base.pendingControlFlags;
                        u32 mask = ~1;
                        u32 value;
                        value = *flags;
                        value &= mask;
                        *flags = value;
                    }
                    actor->base.pendingControl = zero;
                    actor->base.state2A8 = zero;
                    actor->base.phase2A9 = zero;
                    actor->base.behavior2AA = three;
                    actor->base.counter2AC = zero;
                    actor->base.field2B1 = one;
                    actor->base.statusFlags |= status;
                }
            }
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
}
