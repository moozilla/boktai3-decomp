#include "scene_actor_reset.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: address lifetimes, register allocation, and instruction scheduling.
// MATCHING_CONCESSION: register-pinning
// confidence=low: targets player zero and starts the contact pursuit control.
extern u8 *gUnk_02000580;
u32 Nav_IsPositionInMesh(void *, void *, void *);
s32 Terrain_ApplyBlocked(void *, void *, void *);
s32 Div(s32, s32);
struct PursuitContext {
    u8 pad[0x628];
    s16 timer, delay;
    u16 flags;
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
    register void **mesh asm("sl");
    struct ContactPosition **point;
    void *terrain;
    void *dest;
    s32 result;
    typeof(mesh) c1;
    u32 k1;
    if (Blocked(a))
        return;
    if (!gUnk_02000580)
        return;
    ctx = a->context;
    group = (u8 *)(&a->group);
    ready = HasFlag(ctx, 1);
    if (ready)
        return;
    asm("" : "+r"(mesh));
    mesh = (void **)(group + 0x1FC);
    c1 = mesh;
    if (!Nav_IsPositionInMesh(*mesh, &a->position, gUnk_02000580 + 0x30))
        return;
    point = (void *)(group + (k1 = 0x200));
    *(*point) = a->position;
    (*point)->pad = ready;
    a->fieldDC = (u32)(gUnk_02000580 + 0x24);
    SetState(a, 0, 0, 7);
    SetControl(a, a->entryTarget228, 1);
    terrain = *c1;
    dest = (void *)(a->fieldDC + 12);
    if (Nav_IsPositionInMesh(terrain, &a->position, dest) &&
        (result = Terrain_ApplyBlocked(terrain, &a->position, dest))) {
        if (result != 2)
            Memory_Clear(&a->motion, 8);
    } else {
        ClearMotion_BeginPlayerPursuit(a, k1);
    }
    SetFlags(a, 0x4010);
    a->field2C2 = Div(64, 3);
    {
        u32 mask = 2;
        ctx->flags = mask | ctx->flags;
    }
    ctx->timer = 0;
}

#else
INCLUDE_ASM("asm/nonmatching", Spider_BeginPlayerPursuit);
#endif
