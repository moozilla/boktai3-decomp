#include "scene_actor_reset.h"
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void SceneActorReaction_ApplyThreshold(void);
void Skeleton_InitializeCategoryProfile(void *);
void Skeleton_InitVariantMotion(void *);
void Collider_SetAttackProperties(void *, u16, u16, u32, u32, u16);
void sub_08127590(void *, void *, u32, u32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];

static inline u8 Sub080BA268_Take(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
static inline s32 Sub080BA268_Scale6(s32 x)
{
    if (x >= 0)
        return x >> 6;
    return -((-x) >> 6);
}
static inline s32 Sub080BA268_Scale12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
static inline u8 Sub080BA268_RandByte(void)
{
    u32 index = (gUnk_03005308 + 1) & 1023;
    gUnk_03005308 = index;
    return ((const u8 *)gUnk_0203B400)[index * 2];
}
static inline void Sub080BA268_Polar(s32 angle, struct ContactPosition *v, s32 radius)
{
    v->x = Sub080BA268_Scale12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Sub080BA268_Scale12(gUnk_086149C4[angle & 255] * radius);
}

#ifdef NONMATCHING
// fuzzy: 58.1%
// scope shape 143 (differences once low registers are set aside)
// confidence=low: timed attack-recovery state; plays a directional entry, spawns two effect bursts, then queues the next control.
void sub_080BA268(struct SceneActorRegistryResetView *p)
{
    struct ContactPosition v1;
    struct ContactPosition v2;
    struct ContactPosition pos;
    s32 angle;
    u8 *ctx = p->context;

    if (Sub080BA268_Take(&p->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry(p, 0x16);
        angle = Sub080BA268_Scale6((s32)((p->direction + 0x10) & 0xFF)) << 6;
        Sub080BA268_Polar(angle, &v1, 20);
        Sub080BA268_Polar(Sub080BA268_RandByte(), &v2, 10);
        v2.y = 30;
        pos = p->position;
        *(u32 *)&pos = (*(u32 *)&pos & 0xFFFF) | ((((*(u32 *)&pos) >> 16) + 0x104) << 16);
        pos.x += v2.x;
        pos.y += v2.y;
        pos.z += v2.z;
        sub_08127590(&pos, &v2, 0xD45E, 0x1DC3, 2, 30, 2, 0, 316, 0, 0, 0, 0, 0, 0);
        Sub080BA268_Polar(Sub080BA268_RandByte(), &v2, 10);
        v2.y = 30;
        pos.y += 0xDC;
        pos.x += v2.x;
        pos.y += v2.y;
        pos.z += v2.z;
        sub_08127590(&pos, &v2, 0xD45E, 0x1DC3, 3, 30, 2, 0, 316, 0, 0, 0, 0, 0, 0);
        p->category = 5;
        Skeleton_InitializeCategoryProfile(p);
        Skeleton_InitVariantMotion(p);
        Collider_SetAttackProperties(ctx + 0x678, *(u16 *)p->unknownBA, *(u16 *)p->unknownC2, 0,
                                     *(u32 *)p->unknownD8, *(u16 *)p->unknownCA);
    }
    if (p->field2B3) {
        void (*f)(void) = SceneActorReaction_ApplyThreshold;
        u32 k = 0x3e;
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
#else
INCLUDE_ASM("asm/nonmatching", sub_080BA268);
#endif
