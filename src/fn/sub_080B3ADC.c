
#ifdef NONMATCHING
// fuzzy: 91.4%
// foundry draft (worker best 90.4%); what differs is mostly register allocation and instruction order
#define SceneActor_DispatchContextAction sub_080B3ADC
#define SceneActor_IsFacingNearbyTarget  sub_080B335C
#include "global.h"
#include "scene_actor_reset.h"
extern const u8 gUnk_0824E230[];
void sub_080B3110(void *, u32, const u8 *, u32);
void sub_080B23A0(void *, u32);
void sub_080B2858(void *, u32);
void Vorn_SelectRecoveryAction(void *, const u8 *, u32);
u8 sub_080B335C(void *);
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
u32 sub_080B3ADC(struct SceneActorRegistryResetView *p, u32 reset)
{
    u16 *flags;
    u32 first;
    if (TestActor(p, 1)) {
        if (reset == 1) {
            u32 mask = 2;
            p->field69E = 0;
            p->flags6D4 |= mask;
        }
        sub_080B3110(p, reset, gUnk_0824E230, 0x60B);
        return 1;
    }
    {
        u32 mask = 4;
        u16 *ptr = &p->flags6D4;
        first = *ptr & mask;
        flags = ptr;
        first = TestValue(first);
    }
    if (first) {
        if (reset == 1)
            p->field6AC = 0;
        sub_080B23A0(p, reset);
        return 1;
    }
    if (Test(flags, 0x10)) {
        if (reset == 1)
            p->field6D0 = 0;
        if (sub_080B335C(p)) {
            Vorn_SelectRecoveryAction(p, gUnk_0824E230, 0x61F);
            return 1;
        }
        sub_080B2858(p, reset);
        return 1;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080B3ADC);
#endif
