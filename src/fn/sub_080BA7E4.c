#include "global.h"
#include "scene_actor_reaction.h"

extern const s16 gUnk_086149C4[];
void Skeleton_UpdateAlternateAttackRecovery(void);
s32 SceneParticleBurst_CreateWithPause(const struct ContactPosition *,
                                       const struct ContactPosition *, const void *, s32, u32, u32);
s32 Collider_Refresh(void *);
s32 Mod(s32, s32);

static inline s32 Truncate(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline void HeadingStep(u8 *heading, u8 *speedp, struct ContactPosition *v)
{
    u32 direction = *heading;
    u32 speed = *speedp;
    v->x = Truncate(gUnk_086149C4[(direction + 0x40) & 0xFF] * speed);
    v->y = 0;
    v->z = Truncate(speed * gUnk_086149C4[direction]);
}
static inline void AddHalf(s16 *dst, const s16 *src)
{
    *dst += *src;
}
static inline void Clear(const void *src, void *dst, u32 ctl)
{
    CpuSet(src, dst, ctl);
}
static inline void AddZ(s16 *dst, const struct ContactPosition *src)
{
    *dst += src->z;
}
static inline s32 Half(s32 value)
{
    return value >= 0 ? value >> 1 : -((-value) >> 1);
}

void Skeleton_TickDecayingLunge(struct SceneActorRegistryResetView *p, s32 tick_)
{
    u8 *context;
    s32 tick = tick_;
    struct {
        u32 zero;
        struct ContactPosition step;
    } scratch;
    struct ContactPackedVector jitter;
    struct ContactPosition empty;
    struct ContactPosition *np;
    context = *(u8 **)((u8 *)p + 0x3D0);
    if (SceneActorReaction_TakePrepared(p)) {
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)p, 0x15);
        context[0x6D1] = 0x48;
    }
    HeadingStep(&p->body.direction, context + 0x6D1, &scratch.step);
    {
        struct ContactPosition *v = &scratch.step;
        AddHalf(&p->body.motion.x, &v->x);
        v = &p->body.motion;
        AddZ(&v->z, &scratch.step);
    }
    {
        u32 mask = 0x2000;
        u32 *fl = &p->flags2CC;
        *fl |= mask;
    }
    if (Mod(tick, 4) == 0) {
        u32 z;
        s32 h = Half(context[0x6D1]);
        u8 *cp = context + 0x6D1;
        z = 0;
        *cp = h;
        jitter.x = 0x52;
        jitter.y = 0;
        jitter.z = 0x52;
        np = &empty;
        scratch.zero = z;
        Clear(&scratch.zero, np, 0x05000002);
        SceneParticleBurst_CreateWithPause(&p->body.position, np, &jitter, 1, 2, 4);
    }
    Collider_Refresh(context + 0x678);
    if (p->field2B3 && context[0x6D1] == 0) {
        void (*f)(void) = Skeleton_UpdateAlternateAttackRecovery;
        u32 k = 0x2A;
        u32 *w;
        s32 m;
        p->field2B2 = 1;
        p->field2B0 = 0;
        p->field2B5 = k;
        p->target = f;
        p->field2C8 = 0;
        w = &p->flags114;
        m = -2;
        *w = m & *w;
        p->word104 = 0;
    }
}
