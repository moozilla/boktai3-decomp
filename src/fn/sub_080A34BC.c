#include "global.h"

#include "scene_actor_reset.h"
#include "engine.h"

// confidence=low: spider action tick; restores entry state, picks a route or delayed recovery each frame.

// confidence=low: spider action tick; restores entry state, picks a route or delayed recovery each frame.
struct F080A34BC_Ctx {
    u8 pad[0x62A];
    u16 delay;
    u16 flags;
};

void Spider_SelectRandomEntry(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorPlacement_TriggerTetherAction(void *);
s32 SceneActor_IsWithinTetherRange(void *, u32, u32);
void Spider_SelectRecoveryRoute();

static inline u8 F080A34BC_TakeEntry(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

#ifdef NONMATCHING
// fuzzy: 35.9%
// scope shape 171 (differences once low registers are set aside)
void sub_080A34BC(struct SceneActorRegistryResetView *a)
{
    struct F080A34BC_Ctx *ctx = a->context;
    u32 idx;
    u32 next;

    if (F080A34BC_TakeEntry(&a->field2B1)) {
        a->field2B2 = 1;
        a->field2B0 = 0;
        a->field2B5 = 0;
        a->target = a->savedTarget;
        a->field2C8 = 0;
        a->flags114 &= ~1;
        a->word104 = 0;
        a->field2C2 = 0x40;
    }

    if ((u8)Callback_InvokeActor(a, a->contactQuery.opaque))
        return;

    if (ctx->flags & 4) {
        if (!SceneActor_IsWithinTetherRange(a, 0x40, 0x40)) {
            SceneActorPlacement_TriggerTetherAction(a);
            return;
        }
        a->field2C2 = 0x40;
        Spider_SelectRandomEntry((u8 *)&a->group.common + 0x74);
        SceneActorMovement_ResolveRoutePosition(a);
        ctx->delay = 0;
        a->state2A8 = 0;
        a->phase2A9 = 0;
        a->behavior2AA = 15;
        a->counter2AC = 0;
        a->field2B1 = 1;
        a->field2B2 = 1;
        a->field2B0 = 0;
        a->field2B5 = 0x22;
        a->target = (void *)Spider_SelectRecoveryRoute;
        a->field2C8 = 0;
        a->flags114 &= ~1;
        a->word104 = 0;
        return;
    }

    if ((s32)a->counter2AC <= 60) {
        a->counter2AC++;
        return;
    }

    Spider_SelectRandomEntry((u8 *)&a->group.common + 0x74);
    SceneActorMovement_ResolveRoutePosition(a);

    if (a->paletteOffset == 0) {
        idx = (gRandomIndex + 1) & 0x3FF;
        gRandomIndex = idx;
        if ((gRandomTable[idx] & 3) == 0)
            goto recoveryC;
    } else {
        idx = (gRandomIndex + 1) & 0x3FF;
        gRandomIndex = idx;
        if ((gRandomTable[idx] & 7) != 0)
            goto recoveryC;
    }
    a->state2A8 = 0;
    a->phase2A9 = 0;
    a->behavior2AA = 15;
    a->counter2AC = 0;
    a->field2B1 = 1;
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = 0x22;
    a->target = (void *)Spider_SelectRecoveryRoute;
    a->field2C8 = 0;
    a->flags114 &= ~1;
    a->word104 = 0;
    next = (idx + 1) & 0x3FF;
    gRandomIndex = next;
    ctx->delay = (gRandomTable[next] & 0x7F) + 60;
    return;
recoveryC:
    a->state2A8 = 0;
    a->phase2A9 = 0;
    a->behavior2AA = 7;
    a->counter2AC = 0;
    a->field2B1 = 1;
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = 1;
    a->target = a->entryTarget228;
    a->field2C8 = 0;
    a->flags114 &= ~1;
    a->word104 = 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080A34BC);
#endif
