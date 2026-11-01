// MATCHING_CONCESSION: register-pinning
#include "global.h"

#include "scene_actor_reset.h"
#include "engine.h"

// confidence=low: spider action tick; restores entry state, picks a route or delayed recovery each frame.
struct SpiderRecoveryContext {
    u8 pad[0x62A];
    u16 delay;
    u16 flags;
};

void Spider_SelectRandomEntry(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorPlacement_TriggerTetherAction(void *);
s32 SceneActor_IsWithinTetherRange(void *, u32, u32);
void Spider_SelectRecoveryRoute();
// Concession: a single-use statement macro with no evidence that the original had it. Its loop
// notes make flow count the table's set twice (scope fact 26), so global alloc takes the table
// before the 0x3FF mask and the table gets ip, as in the ROM. Ordinary C still needs a real reason
// for those refs (a use of the table at depth 2, or a helper the original shared).
#define SPIDER_CAPTURE_RANDOM_TABLE(out_)                                                          \
    do {                                                                                           \
        (out_) = gRandomTable;                                                                     \
    } while (0)

static inline u8 TakeEntry(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u32 Random(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return table[gRandomIndex];
}
static inline u8 TestFlags(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
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

void sub_080A34BC(struct SceneActorRegistryResetView *a)
{
    u8 *group = (u8 *)&a->group.common;
    struct SpiderRecoveryContext *ctx = a->context;

    if (TakeEntry(&a->field2B1)) {
        SetControl(a, a->savedTarget, 0);
        a->field2C2 = 0x40;
    }
    if ((u8)Callback_InvokeActor(a, a->contactQuery.opaque) == 0) {
        u32 four = 4;
        if (TestFlags(ctx->flags, four)) {
            if ((u8)SceneActor_IsWithinTetherRange(a, 0x40, 0x40)) {
                a->field2C2 = 0x40;
                Spider_SelectRandomEntry(group + 0x74);
                SceneActorMovement_ResolveRoutePosition(a);
                ctx->delay = 0;
                SetState(a, 0, 0, 15);
                SetControl(a, Spider_SelectRecoveryRoute, 0x22);
            } else {
                SceneActorPlacement_TriggerTetherAction(a);
            }
        } else if ((s32)a->counter2AC > 60) {
            Spider_SelectRandomEntry(group + 0x74);
            SceneActorMovement_ResolveRoutePosition(a);
            if (a->paletteOffset == 0) {
                const u16 *table;
                SPIDER_CAPTURE_RANDOM_TABLE(table);
                if ((Random(table) & 3) != 0) {
                    SetState(a, 0, 0, 15);
                    SetControl(a, Spider_SelectRecoveryRoute, 0x22);
                    ctx->delay = (Random(table) & 0x7F) + 60;
                } else {
                    SetState(a, 0, 0, 7);
                    SetControl(a, a->entryTarget228, 1);
                }
            } else {
                if ((Random(gRandomTable) & 7) == 0) {
                    SetState(a, 0, 0, 15);
                    SetControl(a, Spider_SelectRecoveryRoute, 0x22);
                    ctx->delay = (Random(gRandomTable) & 0x7F) + 60;
                } else {
                    SetState(a, 0, 0, 7);
                    SetControl(a, a->entryTarget228, 1);
                }
            }
        } else {
            a->counter2AC++;
        }
    }
}
