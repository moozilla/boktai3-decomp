// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
static inline u8 IsUnavailable(struct SceneActorRegistryActor *input, u32 disabled, u32 excluded)
{
    struct SceneActorRegistryActor *actor = input;
    u32 maskDisabled = disabled;
    register u32 maskExcluded asm("r9") = excluded;
    register u32 offset asm("r1") = 0x2d4;
    u32 value;
    register u32 mask asm("r2");
    value = *(u16 *)((u8 *)actor + offset);
    mask = maskDisabled;
    value &= mask;
    if (value)
        return 1;
    offset -= 4;
    {
        register u32 *field asm("r0") = (void *)((u8 *)actor + offset);
        value = *field;
    }
    mask = maskExcluded;
    asm("" : "+r"(mask));
    value &= mask;
    if (value || actor->health <= 0)
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
        register u32 excluded asm("r9") = 0x20000;
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
                    {
                        u32 offset = 0x104;
                        u32 *field;
                        field = (void *)((u8 *)actor + offset);
                        *field = zero;
                    }
                    {
                        u32 offset = 0x2a8;
                        register u8 *field asm("r0");
                        field = (u8 *)actor + offset;
                        *field = zero;
                        {
                            u32 phaseOffset = 0x2a9;
                            field = (u8 *)actor + phaseOffset;
                            *field = zero;
                        }
                        offset += 2;
                        field = (u8 *)actor + offset;
                        {
                            u32 state = three;
                            *field = state;
                        }
                        offset += 2;
                        field = (u8 *)actor + offset;
                        *(u32 *)field = zero;
                        {
                            register u32 enterOffset asm("r1") = 0x2b1;
                            asm("" : "+r"(enterOffset));
                            field = (u8 *)actor + enterOffset;
                            *field = one;
                        }
                        offset += 0x24;
                        {
                            register u32 *flags asm("r1") = (void *)((u8 *)actor + offset);
                            u32 value;
                            value = *flags;
                            value |= status;
                            *flags = value;
                        }
                    }
                }
            }
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
}
