#include "scene_actor_reset.h"
#define FIELD(p, o, T) (*(T *)((u8 *)(p) + (o)))
extern const s16 gUnk_086149C4[];
void sub_080A9DB0(void *, s32, s32);
void TentacleActor_SelectWanderRoute(void *);
static inline u8 TakeEntry(struct ChaseActor *p)
{
    if (p->base.field2B1) {
        p->base.field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void SetControl(struct ChaseActor *p, void *control, u32 code)
{
    u8 *entry = &p->base.field2B2;
    u32 zero = 0;
    *entry = 1;
    p->base.field2B0 = zero;
    p->base.field2B5 = code;
    p->base.target = control;
    p->base.field2C8 = zero;
    {
        u32 *flags = &p->base.flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    p->base.word104 = zero;
}
static inline void SetState(struct ChaseActor *p, u32 b)
{
    u32 behavior = b;
    p->base.state2A8 = 0;
    p->base.phase2A9 = 0;
    p->base.behavior2AA = behavior;
    p->base.counter2AC = 0;
    p->base.field2B1 = 1;
}
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
void TentacleActor_UpdateKnockbackSlide(struct ChaseActor *p)
{
    if (TakeEntry(p)) {
        struct ContactPosition *velocity;
        u32 angle;
        u8 *direction;
        u32 factor, raw;
        SetControl(p, FIELD(p, 0x22c, void *), 0);
        direction = (u8 *)p + 0x2b6;
        raw = FIELD(p, 0x174, u16);
        velocity = (void *)((u8 *)p + 0x2ec);
        angle = *direction;
        factor = raw >> 3;
        velocity->x = Scale(gUnk_086149C4[(angle + 64) & 255] * factor);
        velocity->y = 0;
        velocity->z = Scale(gUnk_086149C4[angle] * factor);
        FIELD(p, 0x2ee, u16) = 0x20;
        FIELD(p, 0x2e8, u16) = 2;
    }
    sub_080A9DB0(p, 6, 0x1000);
    {
        u32 mask = 16;
        if (p->base.flags2D0 & mask) {
            TentacleActor_SelectWanderRoute(p);
            SetControl(p, p->base.onAlternateEntry, 1);
            SetState(p, 2);
            Sound_PlaySong(983);
        } else {
            p->base.counter2AC++;
        }
    }
}
