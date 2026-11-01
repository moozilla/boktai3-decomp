#include "global.h"
// MATCHING_CONCESSION: register-pinning

// confidence=low: state advances segment interpolation and oscillation before restoring control.

// confidence=low: inferred from segment state and collider updates.

void TentacleBody_UpdateChargeSpread(void);
void TentacleActor_SelectWanderRoute(void *);
void sub_080A9DB0(void *, s32, s32);
void TentacleActor_UpdateOscillation(void *, s32, s32);
#define B(o) (*(u8 *)(p + (o)))
#define H(o) (*(u16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))

static inline u8 TakeEntry(u8 *p)
{
    if (B(0x2b1)) {
        B(0x2b1) = 0;
        return 1;
    }
    return 0;
}

#include "scene_actor_reset.h"
static inline void SetControl(struct SceneActorRegistryResetView *s, void *control, u32 code)
{
    u32 zero;
    u32 *flags;
    u8 *field = &s->field2B2;
    zero = 0;
    *field = 1;
    s->field2B0 = zero;
    s->field2B5 = code;
    s->target = control;
    s->field2C8 = zero;
    flags = &s->flags114;
    {
        u32 mask = -2;
        *flags &= mask;
    }
    s->word104 = zero;
}
static inline void SetState(struct SceneActorRegistryResetView *p, u32 b)
{
    u32 behavior = b;
    p->state2A8 = 0;
    p->phase2A9 = 0;
    p->behavior2AA = behavior;
    p->counter2AC = 0;
    p->field2B1 = 1;
}
static inline s32 Abs(s32 x)
{
    s32 r = x;
    if (x < 0)
        r = -x;
    return r;
}
static inline void Advance(void *p, s32 x, s32 z)
{

    s32 k = 0x6400;
    asm("" : "+r"(k));
    sub_080A9DB0(p, 4, k);
}
void TentacleActor_UpdateRetraction(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *s = input;
    if (TakeEntry((u8 *)s)) {
        SetControl(s, TentacleBody_UpdateChargeSpread, 0x23);
        *(u16 *)((u8 *)s + 0x2e8) = 8;
    }
    {
        s32 x = *(s16 *)((u8 *)s + 0x2ec);
        s32 ax = x;
        s32 z;
        s32 az;
        if (x < 0)
            ax = -x;
        z = *(s16 *)((u8 *)s + 0x2f0);
        az = z;
        if (z < 0)
            az = -z;
        asm volatile("" ::"r"(ax), "r"(az), "r"(x), "r"(z));
        Advance(s, ax, az);
    }
    TentacleActor_UpdateOscillation(s, 0x1ae, 0x28);
    if (s->field2B0) {
        TentacleActor_SelectWanderRoute(s);
        SetControl(s, s->entryTarget228, 1);
        SetState(s, 2);
    } else {
        s->counter2AC++;
    }
}
