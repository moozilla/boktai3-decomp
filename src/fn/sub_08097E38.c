#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"
struct GhostPlayerReaction {
    u8 unknown0[0x558];
    u32 flags;
    u8 unknown55C[0x5BA - 0x55C];
    u8 delay;
    u8 unknown5BB[0x604 - 0x5BB];
    struct ContactPosition previousPlayer;
    u8 unknown60C[0x622 - 0x60C];
    u16 interval, timer;
};
static inline u8 Blocked(struct SceneActorRegistryResetView *s)
{
    if (!s)
        goto zero;
    {
        u32 mask = 0x40;
        if (s->flags2D4 & mask)
            goto one;
    }
    {
        u32 mask = 0x20000;
        if (s->flags2D0 & mask)
            goto one;
    }
    if (s->health > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}
static inline u32 Flags(struct SceneActorRegistryResetView *s, u32 mask)
{
    return s->flags2D4 & mask;
}
static inline u8 HasFlag(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u8 ReactionFlag(struct SceneActorReactionMotion *reaction, u32 mask)
{
    if (reaction->flags & mask)
        return 1;
    return 0;
}
static inline u8 ContextFlag(struct GhostPlayerReaction *state, u32 input, u32 **saved)
{
    u32 mask = input;
    u32 *flags = &state->flags;
    u32 tested;
    tested = *flags & mask;
    *saved = flags;
    if (tested)
        return 1;
    return 0;
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline void State(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                         u32 behavior, u32 counter)
{
    u32 selected = behavior;
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = selected;
    actor->counter2AC = counter;
    actor->field2B1 = 1;
}

// Remaining difference: one player-slot literal reload.
// confidence=low: reacts to player motion within range, with separate drain and approach transitions.
static inline void ZeroState(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    u8 *field = &actor->state2A8;
    u32 zero = 0;
    *field = zero;
    actor->phase2A9 = zero;
    actor->behavior2AA = behavior;
    actor->counter2AC = zero;
    actor->field2B1 = 1;
}
void Ghost_ReactToPlayerMovement(struct SceneActorRegistryResetView *actor)
{
    struct GhostPlayerReaction *state;
    struct ContactPosition delta;
    s32 height;
    u32 active;
    u32 *savedFlags;
    struct SceneActorReactionMotion *reaction;
    if (Blocked(actor))
        return;
    if (Flags(actor, 0x20))
        return;
    state = actor->context;
    {
        u8 **players = &(*(u8 **)gPlayers);
        u8 **slots;
        slots = players;
        Delta(&delta, &state->previousPlayer, (void *)((*(u8 **)gPlayers) + 0x30));
    }
    state->previousPlayer = *(struct ContactPosition *)((*(u8 **)gPlayers) + 0x30);
    if (state->delay) {
        state->delay--;
        return;
    }
    if (Flags(actor, 0x100))
        return;
    if (!delta.x && !delta.z)
        return;
    reaction = (void *)&actor->tracking.target;
    if (!ReactionFlag(reaction, 2))
        return;
    {
        u32 mask = ~2;
        reaction->flags = mask & reaction->flags;
    }
    height = actor->body.position.y - *(s16 *)((*(u8 **)gPlayers) + 0x32);
    if (height < 0)
        height = -height;
    if (height > 512)
        return;
    if (actor->tracking.distance >= actor->tracking.limit)
        return;
    active = ContextFlag(state, 0x10, &savedFlags);
    if (active) {
        if (*(u16 *)((*(u8 **)gPlayers) + 0x428) == 0)
            return;
        ZeroState(actor, 4);
        state->timer = state->interval;
    } else {
        {
            u32 mask = 0x10;
            *savedFlags |= mask;
        }
        State(actor, active, 2, 1, active);
        state->timer = state->interval;
        state->delay = 10;
    }
}
