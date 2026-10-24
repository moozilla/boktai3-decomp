#include "orbit_motion.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// foundry draft (worker best 95.5%); what differs is mostly register allocation and instruction order
#define gSineTable gUnk_086149C4
extern const s16 gUnk_086149C4[];
static inline s32 half(s32 n)
{
    if (n >= 0)
        return n >> 1;
    return -((-n) >> 1);
}
struct OrbitWavePair {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
static inline void waves(struct OrbitVector *v, struct OrbitWavePair *p)
{
    s32 b;
    s32 a;
    a = p->x;
    b = p->y;
    v->x = half(((b + a) << 5) / 3);
    v->y = 0;
    v->z = half(((b - a) << 5) / 3);
}
inline static void waves_UpdateCrossedSines(struct OrbitVector *v, struct OrbitWavePair *p)
{
    s32 b;
    s32 a;
    a = p->x;
    b = p->y;
    v->y = 0;
    v->x = half(((b + a) << 5) / 3);
    v->z = half(((b - a) << 5) / 3);
}

s32 OrbitMotion_UpdateCrossedSines(struct OrbitMotion *p)
{
    struct OrbitWavePair pair;
    pair.x = scale12(p->radius * gUnk_086149C4[p->timer & 255]);
    pair.y = scale12(p->radius2 * gUnk_086149C4[(p->timer * 2) & 255]);
    waves_UpdateCrossedSines(&p->offset, &pair);
    add(p->position, &p->offset, &p->base);
    if (((u32)(++p->timer)) > 512) {
        p->timer = 0;
        return 1;
    }
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", OrbitMotion_UpdateCrossedSines);
#endif
