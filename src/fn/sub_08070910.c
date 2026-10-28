// MATCHING_CONCESSION: register-pinning
#include "global.h"

#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorControl_OnIdle(u8 *);

static inline u8 IsUnavailable(struct SceneActorRegistryActor *actor, u16 **saved, u32 excluded)
{
    u16 *flags = &actor->flags2D4;
    register u32 value asm("r0") = *flags;
    register u32 mask asm("r2") = 0x40;
    value &= mask;
    *saved = flags;
    if (value)
        goto one;
    value = actor->flags2D0;
    mask = excluded;
    value &= mask;
    if (value)
        goto one;
    if (actor->health > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}

static inline u8 HasMovementFlag(struct SceneActorRegistryActor *actor, u32 mask)
{
    if (actor->flags2C0 & mask)
        return 1;
    return 0;
}

static inline void ResetState(struct SceneActorRegistryActor *actor, u32 state, u32 zero)
{
    actor->state2A8 = state;
    actor->phase2A9 = zero;
    actor->behavior2AA = zero;
    actor->counter2AC = zero;
    actor->field2B1 = 1;
}

static inline void ResetControl(struct SceneActorRegistryActor *actor, const void *callback,
                                u32 substate, u32 zero)
{
    actor->field2B2 = 1;
    actor->field2B0 = zero;
    actor->unknown2B5[0] = substate;
    actor->control = callback;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        u32 value = *flags;
        value &= mask;
        *flags = value;
    }
    actor->field104 = zero;
}

// scope shape 67 (differences once low registers are set aside)
// confidence=low: walks the scene actor list and, for available actors with flag 0x100 in flags2C0, resets their script state.
void sub_08070910(void)
{
    struct SceneActorRegistryNode *node;
    struct SceneActorRegistryActor *actor;
    s32 keyword;

    if (!SceneActorRegistry_GetManager())
        return;
    node = gSceneActorRegistryNode;
    actor = node->actor;
    keyword = Script_GetKeywordValue(0x6d, 0);
    if (!actor)
        return;
    {
        u32 movementMask = 0x100;
        u32 zero = 0;
        const void *callback = SceneActorControl_OnIdle;
        u32 substate = 16;
        u32 excluded = 0x20000;
        do {
            u16 *flags;
            u32 changeMask;
            u32 unavailable = IsUnavailable(actor, &flags, excluded);
            if (!unavailable) {
                if (HasMovementFlag(actor, movementMask)) {
                    ResetState(actor, 2, zero);
                    if (keyword == 0) {
                        ResetControl(actor, callback, substate, keyword);
                    }
                    {
                        changeMask = 0x4000;
                        {
                            register u32 mask asm("r0") = changeMask;
                            u32 value = *flags;
                            mask |= value;
                            *flags = mask;
                        }
                    }
                    {
                        changeMask = 8;
                        actor->flags2D0 = actor->flags2D0 | changeMask;
                    }
                }
            }
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
}
