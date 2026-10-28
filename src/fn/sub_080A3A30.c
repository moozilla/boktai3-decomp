#include "scene_actor_reset.h"
#include "player.h"

// MATCHING_CONCESSION: register-pinning
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
static inline u8 Blocked(struct SceneActorRegistryResetView *a)
{
    if (a) {
        u32 mask = 0x40;
        if (a->flags2D4 & mask)
            return 1;
        mask = 0x20000;
        if (a->flags2D0 & mask)
            return 1;
        if (a->health <= 0)
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
static inline void SetState(struct SceneActorRegistryResetView *a, u8 state, u8 phase, u8 behavior)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
static inline void SetControl(struct SceneActorRegistryResetView *a, void *callback, u32 kind)
{
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = kind;
    a->target = callback;
    a->field2C8 = 0;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = 0;
}
static inline void ClearMotion(struct SceneActorRegistryResetView *a, u32 mask)
{
    a->flags2D4 &= ~mask;
}
static inline void SetFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    a->flags2CC = mask | a->flags2CC;
}
inline static void ClearMotion_BeginPlayerPursuit(struct SceneActorRegistryResetView *a, u32 mask)
{
    {
        s32 v1 = (~mask) & a->flags2D4;
        a->flags2D4 = v1;
    }
}

void Spider_BeginPlayerPursuit(struct SceneActorRegistryResetView *a)
{
    struct PursuitContext *ctx;
    u8 *group;
    u8 ready;
    void **mesh;
    struct ContactPosition **point;
    void *terrain;
    void *dest;
    s32 result;
    register u32 motionMask asm("r9");
    if (Blocked(a))
        return;
    if (!(*(u8 **)gPlayers))
        return;
    ctx = a->context;
    group = (u8 *)(&a->group);
    ready = HasFlag(ctx, 1);
    if (ready)
        return;
    mesh = &((struct SpiderPursuitGroup *)group)->mesh;
    if (!Nav_IsPositionInMesh(*mesh, &a->position, (*(u8 **)gPlayers) + 0x30))
        return;
    motionMask = 0x200;
    point = &((struct SpiderPursuitGroup *)group)->point;
    *(*point) = a->position;
    (*point)->pad = ready;
    a->fieldDC = (u32)((*(u8 **)gPlayers) + 0x24);
    SetState(a, 0, 0, 7);
    SetControl(a, a->entryTarget228, 1);
    terrain = *mesh;
    dest = (void *)(a->fieldDC + 12);
    if (Nav_IsPositionInMesh(terrain, &a->position, dest) &&
        (result = Terrain_ApplyBlocked(terrain, &a->position, dest))) {
        if (result != 2)
            Memory_Clear(&a->motion, 8);
    } else {
        ClearMotion_BeginPlayerPursuit(a, motionMask);
    }
    SetFlags(a, 0x4010);
    a->field2C2 = Div(64, 3);
    {
        u32 mask = 2;
        ctx->flags = mask | ctx->flags;
    }
    ctx->timer = 0;
}
