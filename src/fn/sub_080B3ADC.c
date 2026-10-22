// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"

#ifdef NONMATCHING
// fuzzy: 99.1%
// Remaining: the incoming reset argument is saved before the actor pointer.
extern const u8 gUnk_0824E230[];
void sub_080B3110(void *, u32, const u8 *, u32);
void Vorn_BeginPursuitRoute(void *, u32);
void Vorn_SelectRetreatRoute(void *, u32);
void Vorn_SelectRecoveryAction(void *, const u8 *, u32);
u8 SceneActor_IsFacingNearbyTarget(void *);
static inline u8 Test(u16 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
static inline u8 TestActor(struct SceneActorRegistryResetView *p, u32 mask)
{
    if (p->flags6D4 & mask)
        return 1;
    return 0;
}
static inline u8 TestValue(u32 value)
{
    if (value)
        return 1;
    return 0;
}

u32 SceneActor_DispatchContextAction(struct SceneActorRegistryResetView *p0, u32 reset)
{
    struct SceneActorRegistryResetView *q;
    struct SceneActorRegistryResetView *p;
    u16 *flags;
    u32 first;
    {
        register struct SceneActorRegistryResetView *input asm("r3") = p0;
        asm("" : "+r"(input));
        q = input;
    }
    p = q;
    asm("" : "+r"(q));
    if (TestActor(p, 1)) {
        if (reset == 1) {
            u32 mask = 2;
            q->field69E = 0;
            asm("" : "+r"(mask));
            q->flags6D4 = mask | q->flags6D4;
        }
        sub_080B3110(p, reset, gUnk_0824E230, 0x60B);
        return 1;
    }
    {
        u32 mask = 4;
        u16 *ptr = &q->flags6D4;
        first = *ptr & mask;
        flags = ptr;
        first = TestValue(first);
    }
    if (first) {
        if (reset == 1)
            q->field6AC = 0;
        Vorn_BeginPursuitRoute(p, reset);
        return 1;
    }
    if (Test(flags, 0x10)) {
        if (reset == 1)
            q->field6D0 = 0;
        if (SceneActor_IsFacingNearbyTarget(p)) {
            Vorn_SelectRecoveryAction(p, gUnk_0824E230, 0x61F);
            return 1;
        }
        Vorn_SelectRetreatRoute(p, reset);
        return 1;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", SceneActor_DispatchContextAction);
#endif
