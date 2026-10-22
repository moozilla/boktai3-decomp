#include "global.h"
#include "scene_actor_reset.h"
extern u8 *gUnk_02000580;
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
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
#ifdef NONMATCHING
// fuzzy: 84.4%
// Remaining differences: reaction flag masking and global/context address lifetimes.
// confidence=low: reacts to player motion within range, with separate drain and approach transitions.
#define Ghost_ReactToPlayerMovement sub_08097E38
void Ghost_ReactToPlayerMovement(struct SceneActorRegistryResetView *actor)
{
    struct GhostPlayerReaction *state;
    struct ContactPosition delta;
    s32 height;
    u32 active;
    struct SceneActorReactionMotion *reaction;
    if (Blocked(actor))
        return;
    if (Flags(actor, 0x20))
        return;
    state = actor->context;
    Delta(&delta, &state->previousPlayer, (void *)(gUnk_02000580 + 0x30));
    state->previousPlayer = *(struct ContactPosition *)(gUnk_02000580 + 0x30);
    if (state->delay) {
        state->delay--;
        return;
    }
    if (Flags(actor, 0x100))
        return;
    if (!delta.x && !delta.z)
        return;
    reaction = (void *)&actor->fieldDC;
    if (!HasFlag(reaction->flags, 2))
        return;
    reaction->flags &= ~2;
    height = actor->position.y - *(s16 *)(gUnk_02000580 + 0x32);
    if (height < 0)
        height = -height;
    if (height > 512)
        return;
    if (actor->distanceF0 >= actor->distanceLimitF8)
        return;
    active = HasFlag(state->flags, 0x10);
    if (active) {
        if (*(u16 *)(gUnk_02000580 + 0x428) == 0)
            return;
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 4;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
        state->timer = state->interval;
    } else {
        {
            u32 mask = 0x10;
            state->flags |= mask;
        }
        actor->state2A8 = active;
        actor->phase2A9 = 2;
        actor->behavior2AA = 1;
        actor->counter2AC = active;
        actor->field2B1 = 1;
        state->timer = state->interval;
        state->delay = 10;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08097E38);
#endif
