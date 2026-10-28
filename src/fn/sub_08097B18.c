#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"
struct LinkedReactionContext {
    u8 unknown0[0x558];
    u32 flags;
};
static inline u8 TestFlags(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
void SceneActor_EndLinkedReaction(struct SceneActorRegistryResetView *actor)
{
    struct LinkedReactionContext *state = actor->context;
    {
        u32 mask = 0x100000;
        actor->flags2D0 |= mask;
    }
    {
        u32 *flags = &state->flags;
        u32 mask = ~0x11;
        *flags &= mask;
    }
    {
        struct ContactCollider *collider = &actor->colliderA;
        collider->flags &= 0xFFFB;
    }
    {
        u32 mask = 0x80;
        if (TestFlags(state->flags, mask)) {
            u8 *count = (*(u8 **)gPlayers) + 0x4F6;
            *count -= 3;
            {
                u32 *flags = &state->flags;
                u32 mask = ~0x80;
                *flags &= mask;
            }
        }
    }
}
