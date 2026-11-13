#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
static inline u8 IsUnavailable(struct SceneActorRegistryActor *actor, u32 disabled, u32 excluded)
{
    if ((actor->flags2D4 & disabled) || (actor->flags2D0 & excluded) || actor->health <= 0)
        return 1;
    return 0;
}

/* Reset the observed control/state fields for eligible categories4,1,3.
 * Behavior3 and substate5 remain numeric: their game roles are unconfirmed. */
void SceneActorRegistry_ResetCategoryControl(void)
{
    struct SceneActorRegistryNode *node;
    struct SceneActorRegistryActor *actor;
    SceneActorRegistry_GetManager();
    node = gSceneActorRegistryNode;
    actor = node->actor;
    if (actor) {
        u32 disabled = 0x40;
        u32 excluded = 0x20000;
        u32 five = 5;
        u32 zero = 0;
        u32 one = 1;
        u32 three;
        u32 status;
        {
            u32 initialThree = 3;
            three = initialThree;
        }
        status = 0x1000;
        do {
            u32 unavailable = IsUnavailable(actor, disabled, excluded);
            if (!unavailable) {
                u32 category = actor->category;
                if (category == 4 || category == 1 || category == 3) {
                    const void *control = actor->control5F4;
                    actor->field2B2 = one;
                    actor->field2B0 = zero;
                    actor->unknown2B5[0] = five;
                    actor->control = control;
                    actor->field2C8 = zero;
                    {
                        u32 *flags = &actor->flags114;
                        u32 mask = ~1;
                        u32 value;
                        value = *flags;
                        value &= mask;
                        *flags = value;
                    }
                    actor->field104 = zero;
                    actor->state2A8 = zero;
                    actor->phase2A9 = zero;
                    actor->behavior2AA = three;
                    actor->counter2AC = zero;
                    actor->field2B1 = one;
                    {
                        u32 *flags = &actor->flags2D0;
                        u32 value = *flags;
                        value |= status;
                        *flags = value;
                    }
                }
            }
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
}
