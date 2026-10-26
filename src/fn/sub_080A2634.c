#include "scene_actor_reset.h"
#include "engine.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: selects delayed route recovery or alternate recovery after the context delay.
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void ScaledShadow_Enable(void *);
void ScaledShadow_Destroy(void *);
void SceneActorReaction_RestoreAlternateAfterDelay(void);
struct SpiderFlagWord {
    s16 bits : 16;
};
struct SpiderRecoveryContext {
    u8 pad[0x62A];
    s16 delay;
    struct SpiderFlagWord flags;
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
static inline void ClearContext(struct SpiderFlagWord *p, u32 mask)
{
    p->bits &= mask;
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
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = kind;
    a->target = callback;
    a->field2C8 = 0;
    {
        u32 *p = &a->flags114;
        u32 mask = ~1;
        *p &= mask;
    }
    a->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *a, u8 state, u8 phase, u8 behavior)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}

s32 SceneActorControl_CreateFromPosition(void *);
static inline s32 Random(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    {
        register const u16 *table asm("r5") = gUnk_0203B400;
        return *(u16 *)(offset + (u32)table);
    }
}
void Spider_SelectRecoveryRoute(struct SceneActorRegistryResetView *a, s32 frame)
{
    struct SpiderRecoveryContext *ctx = a->context;
    u8 *actor = (u8 *)a + 0x50;
    u8 *group;
    TakeEntry(&a->field2B2);
    SceneActorAnimation_SelectDirectionalEntry(a, 14);
    if (frame <= ctx->delay)
        return;
    group = (u8 *)&a->group;
    ClearContext(&ctx->flags, ~4);
    if (a->paletteOffset == 0 && SceneActorControl_CreateFromPosition(actor) >= 0) {
        SaveRecovery((void *)&a->word460, a->entryTarget228, 7, 60);
        SetControl(a, SceneActorReaction_RestoreAlternateAfterDelay, 14);
        SetState(a, 3, 1, 0);
    } else {
        u32 choice = (Random() >> 3) & 3;
        if (choice == 0) {
            SaveRecovery((void *)(group + 0x8C), a->entryTarget228, 7, 60);
            SetControl(a, SceneActorReaction_RestoreAlternateAfterDelay, 14);
            SetState(a, 3, 1, 0);
        } else {
            SaveRecovery((void *)(group + 0x8C), a->savedTarget, 5, 60);
            SetControl(a, SceneActorReaction_RestoreAlternateAfterDelay, 14);
            SetState(a, 3, 1, 0);
        }
    }
}
