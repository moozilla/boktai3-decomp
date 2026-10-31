#include "orbit_motion.h"

extern const s16 gUnk_086149C4[];
static inline s32 OrbitMotion_HalfTowardZero(s32 n)
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
static inline void OrbitMotion_ProjectCrossedPair(struct OrbitVector *v, struct OrbitWavePair *p)
{
    s32 screenY;
    s32 screenX;
    screenX = p->x;
    screenY = p->y;
    {
        s32 projectedX = OrbitMotion_HalfTowardZero(((screenX + screenY) << 5) / 3);
        s16 *destinationX = &v->x;
        *destinationX = projectedX;
    }
    v->y = 0;
    v->z = OrbitMotion_HalfTowardZero(((screenY - screenX) << 5) / 3);
}
#ifdef NONMATCHING
// fuzzy: 97.0%
s32 OrbitMotion_UpdateCrossedSines(struct OrbitMotion *p)
{
    struct OrbitWavePair pair;
    pair.x = scale12(p->radius * gUnk_086149C4[p->timer & 255]);
    pair.y = scale12(p->radius2 * gUnk_086149C4[(p->timer * 2) & 255]);
    OrbitMotion_ProjectCrossedPair(&p->offset, &pair);
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
