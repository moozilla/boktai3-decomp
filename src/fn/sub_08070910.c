#include "global.h"

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
            u16 bits;
            u32 unavailable = IsUnavailable(actor, &flags, excluded);
            if (!unavailable) {
                if (HasMovementFlag(actor, movementMask)) {
                    ResetState(actor, 2, zero);
                    if (keyword == 0) {
                        ResetControl(actor, callback, substate, keyword);
                    }
                    {
                        u32 mask = 0x4000;
                        bits = *flags;
                        mask |= bits;
                        *flags = mask;
                    }
                    {
                        bits = 8;
                        actor->flags2D0 = actor->flags2D0 | bits;
                    }
                }
            }
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
}
