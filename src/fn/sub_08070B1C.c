// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorControl_OnIdle(u8 *);
static inline u8 IsUnavailable(struct SceneActorRegistryActor *input, u16 **saved,
                               u32 excludedInput)
{
    struct SceneActorRegistryActor *actor = input;
    register u32 excluded asm("r10") = excludedInput;
    u16 *field;
    register u32 value asm("r0");
    u32 mask;
    field = &actor->flags2D4;
    value = *field;
    mask = 0x40;
    value &= mask;
    *saved = field;
    if (value)
        goto one;
    {
        u32 *field;
        field = &actor->flags2D0;
        value = *field;
        mask = excluded;
        value &= mask;
        if (value)
            goto one;
    }
    if (actor->health > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}
void SceneActorRegistry_ResetEligibleCategoryState(void)
{
    struct SceneActorRegistryNode *node;
    struct SceneActorRegistryActor *actor;
    SceneActorRegistry_GetManager();
    node = gSceneActorRegistryNode;
    actor = node->actor;
    if (actor) {
        register u32 excluded asm("r10") = 0x20000;
        u32 reject;
        const void *callback;
        u32 substate;
        u32 one;
        u32 zero;
        {
            u32 initial = 0xa007;
            reject = initial;
        }
        {
            const void *initial = (const void *)SceneActorControl_OnIdle;
            callback = initial;
        }
        {
            u32 initial = 16;
            substate = initial;
        }
        do {
            u16 *flags;
            one = 1;
            zero = 0;
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
