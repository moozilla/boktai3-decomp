#include "scene_actor_reset.h"
#define FIELD(p, o, T) (*(T *)((u8 *)(p) + (o)))
extern const s16 gUnk_086149C4[];
void sub_080A9DB0(void *, s32, s32);
void TentacleActor_SelectWanderRoute(void *);
static inline u8 TakeEntry(struct SceneActorRegistryResetView *p)
{
    if (p->field2B1) {
        p->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *p, void *control, u32 code)
{
    u8 *entry = &p->field2B2;
    u32 zero = 0;
    *entry = 1;
    p->field2B0 = zero;
    p->field2B5 = code;
    p->target = control;
    p->field2C8 = zero;
    {
        u32 *flags = &p->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    p->word104 = zero;
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
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
void TentacleActor_UpdateKnockback(struct SceneActorRegistryResetView *p)
{
    if (TakeEntry(p)) {
        struct ContactPosition *velocity;
        u32 angle;
        u32 factor = 4;
        u8 *direction;
        SetControl(p, FIELD(p, 0x22c, void *), 0);
        direction = &p->body.direction;
        velocity = (void *)((u8 *)p + 0x2ec);
        angle = *direction;
        velocity->x = Scale(gUnk_086149C4[(angle + 64) & 255] * factor);
        velocity->y = 0;
        velocity->z = Scale(gUnk_086149C4[angle] * factor);
        FIELD(p, 0x2ee, u16) = 0;
        FIELD(p, 0x2e8, u16) = 2;
    }
    sub_080A9DB0(p, 6, 0x1000);
    {
        u32 mask = 16;
        if (p->flags2D0 & mask) {
            TentacleActor_SelectWanderRoute(p);
            SetControl(p, p->entryTarget228, 1);
            SetState(p, 2);
        } else {
            p->counter2AC++;
        }
    }
}
