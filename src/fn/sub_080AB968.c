#include "global.h"
// MATCHING_CONCESSION: register-pinning

// confidence=low: state advances segment interpolation and oscillation before restoring control.

// confidence=low: inferred from segment state and collider updates.

#ifdef NONMATCHING
// fuzzy: 88.8%
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
static inline s32 Abs(s32 x)
{
    s32 r = x;
    if (x < 0)
        r = -x;
    return r;
}
static inline void Advance(void *p, s32 x, s32 z)
{

    sub_080A9DB0(p, 4, 0x6400);
}
void TentacleActor_UpdateRetraction(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *s = input;
    if (TakeEntry((u8 *)s)) {
        SetControl(s, TentacleBody_UpdateChargeSpread, 0x23);
        *(u16 *)((u8 *)s + 0x2e8) = 8;
    }
    {
        s32 ax = Abs(*(s16 *)((u8 *)s + 0x2ec));
        s32 az = Abs(*(s16 *)((u8 *)s + 0x2f0));
        asm("" ::"r"(ax), "r"(az));
        Advance(s, ax, az);
    }
    TentacleActor_UpdateOscillation(s, 0x1ae, 0x28);
    if (s->field2B0) {
        u32 zero;
        TentacleActor_SelectWanderRoute(s);
        SetControl(s, s->entryTarget228, 1);
        zero = 0;
        {
            register u32 phase asm("r1") = 2;
            register u32 offset asm("r2") = 0x2a8;
            *((u8 *)s + offset) = zero;
            offset++;
            *((u8 *)s + offset) = zero;
            offset++;
            *((u8 *)s + offset) = phase;
        }
        s->counter2AC = zero;
        s->field2B1 = 1;
    } else
        s->counter2AC++;
}

#else
INCLUDE_ASM("asm/nonmatching", TentacleActor_UpdateRetraction);
#endif
