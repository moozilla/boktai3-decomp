#include "global.h"
#include "scene_actor_reaction.h"

extern const s16 gUnk_086149C4[];
void Skeleton_UpdateHeavyAttackRecovery(void);
void MovingSprite_Create(struct ContactPackedVector *, u32, u32, u32, u32, u32, s32);
s32 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);

static inline s32 Truncate(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline void HeadingStep(struct ContactPosition *v, u32 direction, u32 speed)
{
    v->x = Truncate(gUnk_086149C4[(direction + 0x40) & 0xFF] * speed);
    v->y = 0;
    v->z = Truncate(speed * gUnk_086149C4[direction]);
}
static inline void Difference(struct ContactPosition *d, struct ContactPosition *a,
                              struct ContactPosition *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
static inline s32 Wrap(s32 d)
{
    if (d > 0x80)
        d -= 0x100;
    else if (d < -0x80)
        d += 0x100;
    return d;
}
static inline s32 Quarter(s32 value)
{
    return value >= 0 ? value >> 6 : -((-value) >> 6);
}

void Skeleton_UpdateHomingBurst(struct SceneActorRegistryResetView *p, s32 tick)
{
    u8 *context = p->context;
    if (SceneActorReaction_TakePrepared(p)) {
        struct ContactPackedVector spawn = *(struct ContactPackedVector *)&p->body.position;
        struct ContactPosition delta;
        struct ContactPosition step;
        u32 base = (u8)(Quarter((p->body.direction + 0x20) & 0xFF) << 6);
        s32 angle, diff;
        struct ContactPosition *dp = &delta;
        struct ContactPosition *pos = &p->body.position;
        struct ContactPosition *target;
        target = (struct ContactPosition *)(context + 0x710);
        Difference(dp, target, pos);
        angle = Math_Atan2(dp->x, dp->z);
        diff = Wrap(angle - base);
        HeadingStep(&step, base, 10);
        spawn.x += step.x;
        spawn.y += 0x104;
        spawn.z += step.z;
        if (diff > 12)
            base = (u8)(base + 12);
        else if (diff < -12)
            base = (u8)(base - 12);
        else
            base = (u8)angle;
        {
            struct ContactPackedVector *sp = &spawn;
            u32 b8, c0, d4, c8;
            b8 = p->attacks.first[2];
            c0 = p->attacks.second[2];
            d4 = p->attacks.flags[2];
            c8 = p->attacks.parameter[2];
            MovingSprite_Create(sp, base, b8, c0, d4, c8, 0x2A);
        }
        Sound_PlaySong(0x3A8);
    }
    SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)p, 0x14);
    if (tick > 0x18) {
        void (*f)(void) = Skeleton_UpdateHeavyAttackRecovery;
        u32 k = 0x2E;
        u32 z;
        u8 *a = &p->field2B2;
        u32 *w;
        s32 m;
        z = 0;
        *a = 1;
        p->field2B0 = z;
        p->field2B5 = k;
        p->target = f;
        p->field2C8 = z;
        w = &p->flags114;
        m = -2;
        *w = m & *w;
        p->word104 = z;
    }
}
