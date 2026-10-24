// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"

extern const s16 gUnk_086149C4[];
void Skeleton_TickDecayingLunge(void);
s32 SceneParticleBurst_CreateWithPause(const struct ContactPosition *,
                                       const struct ContactPosition *, const void *, s32, u32, u32);
static inline s32 Truncate(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline void HeadingStep(struct ContactPosition *v, u8 *heading, s32 speed)
{
    u32 direction = *heading;
    v->x = Truncate(gUnk_086149C4[(direction + 0x40) & 0xFF] * speed);
    v->y = 0;
    v->z = Truncate(speed * gUnk_086149C4[direction]);
}
static inline void CpuSet_2(int ctl, u32 *src, struct ContactPosition *dst)
{
    CpuSet(src, dst, ctl);
}
static inline void AddHalf(s16 *dst, const s16 *src)
{
    *dst += *src;
}
static inline void AddZ(s16 *dst, const struct ContactPosition *src)
{
    *dst += src->z;
}
void SceneActorReaction_TickForwardBurst(struct SceneActorRegistryResetView *p_)
{
    u8 *heading;
    struct SceneActorRegistryResetView *p = p_;
    struct {
        u32 zero;
        struct ContactPosition step;
    } scratch;
    struct ContactPackedVector jitter;
    struct ContactPosition *step;
    s16 *motionZ;
    if (SceneActorReaction_TakePrepared(p)) {
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)p, 0x14);
        Sound_PlaySong(0x3AB);
    }
    heading = &p->direction;
    HeadingStep(&scratch.step, heading, 32);
    step = &scratch.step;
    /* Keep the motion source separate from the vector cleared for the burst. */
    asm("" : "+r"(step));
    AddHalf(&p->motion.x, &step->x);
    motionZ = &p->motion.z;
    step = &scratch.step;
    AddZ(motionZ, step);
    {
        u32 mask = 0x2000;
        u32 *fl = &p->flags2CC;
        *fl |= mask;
    }
    scratch.zero = 0;
    CpuSet_2(0x05000002, &scratch.zero, step);
    jitter.x = 0x52;
    jitter.y = 0;
    jitter.z = 0x52;
    SceneParticleBurst_CreateWithPause(&p->position, step, &jitter, 1, 2, 4);
    if (p->field2B3) {
        void (*f)(void) = Skeleton_TickDecayingLunge;
        u32 k = 0x29;
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
