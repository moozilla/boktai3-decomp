
#ifdef NONMATCHING
// fuzzy: 83.8%
// foundry draft (worker best 82.2%); what differs is mostly register allocation and instruction order
#define ActorImpact_TestForwardPoint sub_08179F60
#include "global.h"
#include "contact_actor.h"
extern const s16 gUnk_086149C4[];
u8 sub_0821D4F0(void *, void *, u32, u32);
struct ImpactProbeActor { u8 unknown0[0x51]; u8 angle; u8 unknown52[2]; struct ContactPosition position; };
static inline s32 trunc12(s32 v) { if (v >= 0) return v >> 12; return -((-v) >> 12); }
u8 sub_08179F60(struct ImpactProbeActor *p)
{
    struct ContactPosition point;
    u8 *anglePtr = &p->angle;
    s16 *v = (s16 *)&point;
    u32 angle = *anglePtr;
    { s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * 256); s32 zero = 0; v[0] = x; v[1] = zero; }
    { s32 sample = gUnk_086149C4[angle]; s32 speed = 256; v[2] = trunc12(sample * speed); }
    {
        struct ContactPosition *d = &point;
        struct ContactPosition *s = &p->position;
        u8 result;
        point.x = (u16)s->x + (u16)point.x;
        d->y = (u16)d->y + (u16)s->y;
        {u32 z = (u16)s->z; u32 delta = (u16)d->z; d->z = z + delta;}
        switch (sub_0821D4F0(s, &point, 2, 256)) { case 0: result = 0; break; default: result = 1; break; }
        return result;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08179F60);
#endif
