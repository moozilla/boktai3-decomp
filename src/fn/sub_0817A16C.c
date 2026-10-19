
#ifdef NONMATCHING
// fuzzy: 96.9%
// foundry draft (worker best 96.9%); what differs is mostly register allocation and instruction order
#define ActorMovement_FindQuarterTurnPoint sub_0817A16C
#define ActorMovement_TestPoint            ActorPosition_TestPath
#include "global.h"
#include "contact_actor.h"
extern const s16 gUnk_086149C4[];
s32 ActorMovement_TestPoint(void *, void *);
struct MovementSearchActor {
    u8 unknown0[0x51];
    u8 angle;
    u8 unknown52[0xbe];
    struct ContactPosition *center;
};
static inline s32 trunc12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void sub_0817A16C(struct MovementSearchActor *p, struct ContactPosition *out)
{
    struct ContactPosition point;
    struct ContactPosition *center = p->center;
    u32 start = p->angle;
    s32 i = 0;
    u32 mask = 255;
    s16 *v = (s16 *)&point;
    const s16 *sine = gUnk_086149C4;
    do {
        u32 angle = (start + (i << 6)) & mask;
        {
            s32 x = trunc12(gUnk_086149C4[(angle + 64) & mask] * 128);
            s32 zero = 0;
            v[0] = x;
            v[1] = zero;
        }
        {
            s32 sample = sine[angle & mask];
            u32 speed = 128;
            s32 z = trunc12((s32)(speed * sample));
            v[2] = z;
            {
                u32 x = (u16)center->x;
                u32 dx = (u16)v[0];
                v[0] = x + dx;
            }
            v[1] = (u16)v[1] + (u16)center->y;
            v[2] = z + (u16)center->z;
        }
        if (ActorMovement_TestPoint(p, &point)) {
            *out = point;
            return;
        }
        i++;
    } while (i <= 3);
    *out = *center;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0817A16C);
#endif
