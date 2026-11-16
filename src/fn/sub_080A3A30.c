#include "scene_actor_reset.h"
#include "player.h"

// confidence=low: targets player zero and starts the contact pursuit control.
u32 Nav_IsPositionInMesh(void *, void *, void *);
s32 Terrain_ApplyBlocked(void *, void *, void *);
s32 Div(s32, s32);
struct PursuitContext {
    u8 pad[0x628];
    s16 timer, delay;
    u16 flags;
};
struct SpiderPursuitGroup {
    u8 unknown0[0x1FC];
    void *mesh;
    struct ContactPosition *point;
};
static inline u8 Blocked(struct ChaseActor *a)
{
    if (a) {
        u32 mask = 0x40;
        if (a->base.flags2D4 & mask)
            return 1;
        mask = 0x20000;
        if (a->base.flags2D0 & mask)
            return 1;
        if (a->base.health <= 0)
            return 1;
    }
    return 0;
}
static inline u8 HasFlag(struct PursuitContext *c, u32 mask)
{
    if (c->flags & mask)
        return 1;
    return 0;
}
static inline void SetState(struct ChaseActor *a, u8 state, u8 phase, u8 behavior)
{
    a->base.state2A8 = state;
    a->base.phase2A9 = phase;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}
static inline void SetControl(struct ChaseActor *a, void *callback, u32 kind)
{
    a->base.field2B2 = 1;
    a->base.field2B0 = 0;
    a->base.field2B5 = kind;
    a->base.target = callback;
    a->base.field2C8 = 0;
    {
        u32 *flags = &a->base.flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->base.word104 = 0;
}
static inline void SetFlags(struct ChaseActor *a, u32 mask)
{
    a->base.flags2CC = mask | a->base.flags2CC;
}
void Spider_BeginPlayerPursuit(struct ChaseActor *a)
{
    struct PursuitContext *ctx;
    u8 *group;
    u8 ready;
    void **mesh;
    struct ContactPosition **point;
    void *terrain;
    void *dest;
    s32 result;
    u32 motionMask;
    if (Blocked(a))
        return;
    if (!(*(u8 **)gPlayers))
        return;
    ctx = a->base.context;
    group = (u8 *)(&a->group.common);
    ready = HasFlag(ctx, 1);
    if (ready)
        return;
    mesh = &((struct SpiderPursuitGroup *)group)->mesh;
    if (!Nav_IsPositionInMesh(*mesh, &a->base.body.position, (*(u8 **)gPlayers) + 0x30))
        return;
    motionMask = 0x200;
    point = &((struct SpiderPursuitGroup *)group)->point;
    *(*point) = a->base.body.position;
    (*point)->pad = ready;
    a->base.tracking.target = (u32)((*(u8 **)gPlayers) + 0x24);
    SetState(a, 0, 0, 7);
    SetControl(a, a->base.entryTarget228, 1);
    terrain = *mesh;
    dest = &a->base.tracking.target->position;
    if (!Nav_IsPositionInMesh(terrain, &a->base.body.position, dest)) {
        a->base.flags2D4 &= ~motionMask;
    } else {
        result = Terrain_ApplyBlocked(terrain, &a->base.body.position, dest);
        if (result) {
            if (result != 2)
                Memory_Clear(&a->base.body.motion, 8);
        } else {
            a->base.flags2D4 &= ~motionMask;
        }
    }
    SetFlags(a, 0x4010);
    a->base.field2C2 = Div(64, 3);
    ctx->flags |= 2;
    ctx->timer = 0;
}
