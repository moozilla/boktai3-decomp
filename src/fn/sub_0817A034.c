
#ifdef NONMATCHING
// fuzzy: 95.9%
// foundry draft (worker best 94.8%); what differs is mostly register allocation and instruction order
#define ActorImpact_FindNearbyPoint sub_0817A034
#include "global.h"
#include "contact_actor.h"
extern const s16 gUnk_086149C4[];
u8 sub_0821D4F0(void *, void *, u32, u32);
struct ImpactMovementBody { u8 unknown0[12]; struct ContactPosition position; };
struct ImpactSearchActor { u8 unknown0[0x48]; struct ImpactMovementBody body; };
static inline s32 trunc12(s32 v) { if (v >= 0) return v >> 12; return -((-v) >> 12); }
void sub_0817A034(struct ImpactSearchActor *p, struct ContactPosition *out, u32 start)
{
    struct ContactPosition point;
    struct ImpactMovementBody *body = &p->body;
    s32 i = 0;
    u32 mask = 255;
    s16 *v = (s16 *)&point;
    struct ContactPosition *center = &p->body.position;
    do {
        u32 angle = (start + (i << 5)) & mask;
        { s32 x = trunc12(gUnk_086149C4[(angle + 64) & mask] * 256); s32 zero = 0; v[0] = x; v[1] = zero; }
        {
            s32 sample = gUnk_086149C4[angle & mask];
            u16 speed = 256;
            s32 value = sample * speed;
            s32 z;
            if (value >= 0) z = value >> 12;
            else { value = -value; value >>= 12; z = -value; }
            { u32 x = (u16)body->position.x; u32 dx = (u16)v[0]; v[0] = x + dx; }
            v[1] = (u16)v[1] + (u16)center->y;
            v[2] = (u16)center->z + z;
        }
        if (sub_0821D4F0(center, &point, 3, 256)) {
            *out = point;
            return;
        }
        i++;
    } while (i <= 7);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0817A034);
#endif
