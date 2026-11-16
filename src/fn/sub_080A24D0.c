#include "scene_actor_reset.h"
// confidence=low: animation completion restores the alternate control after a delay.
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void ScaledShadow_Enable(void *);
void ScaledShadow_Destroy(void *);
void SceneActorReaction_RestoreAlternateAfterDelay(void);
struct SpiderRecoveryContext {
    u8 pad[0x62C];
    u16 flags;
};
struct SpiderHalfFlags {
    s16 bits : 16;
};
static inline u8 TakeEntry(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline void ClearHidden(struct SpiderHalfFlags *p, u32 mask)
{
    p->bits &= mask;
}
struct RecoveryPacket {
    u8 pad[8];
    u8 state, phase, behavior, gap;
    u32 delay;
    void *callback;
};
static inline void SetCollider(struct ContactCollider *c)
{
    c->flags |= 4;
}
static inline void ClearCollider(struct ContactCollider *c)
{
    u32 mask = ~4;
    c->flags &= mask;
}
static inline void ClearContext(struct SpiderRecoveryContext *c, u32 mask)
{
    c->flags &= mask;
}
static inline void SaveRecovery(struct RecoveryPacket *p, void *callback, u8 behavior, u32 delay)
{
    p->delay = delay;
    p->callback = callback;
    p->state = 0;
    p->phase = 0;
    p->behavior = behavior;
}
static inline void SetControl(struct SceneActorRegistryResetView *a, void *callback, u32 kind)
{
    a->base.field2B2 = 1;
    a->base.field2B0 = 0;
    a->base.field2B5 = kind;
    a->base.target = callback;
    a->base.field2C8 = 0;
    {
        u32 *p = &a->base.flags114;
        u32 mask = ~1;
        *p &= mask;
    }
    a->base.word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *a, u8 state, u8 phase, u8 behavior)
{
    a->base.state2A8 = state;
    a->base.phase2A9 = phase;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}
void Spider_UpdateHiddenRecovery(struct SceneActorRegistryResetView *a)
{
    struct SpiderRecoveryContext *ctx = a->base.context;
    if (TakeEntry(&a->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry(a, 0);
        ScaledShadow_Enable((u8 *)a + 0x360);
        {
            u32 mask = 0x800;
            a->base.flags2C0 = mask | a->base.flags2C0;
        }
        SetCollider(&a->base.colliderB);
        SetCollider(&a->base.colliderA);
    }
    {
        u32 mask = 0x10;
        if (a->base.flags2D0 & mask) {
            u32 zero;
            if (*((u8 *)a + 0x360))
                ScaledShadow_Destroy((u8 *)a + 0x360);
            ClearHidden((void *)&a->base.flags2C0, ~0x800);
            ClearCollider(&a->base.colliderB);
            ClearCollider(&a->base.colliderA);
            SaveRecovery((void *)&a->group.saved.control, a->base.savedTarget, 5, 20);
            SetControl(a, SceneActorReaction_RestoreAlternateAfterDelay, 14);
            SetState(a, 3, 1, 0);
            ClearContext(ctx, ~1);
        }
    }
}
