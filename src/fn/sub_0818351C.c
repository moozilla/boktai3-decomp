#define ActorController_UpdateDecayingVelocity sub_0818351C
#define gSineTable                             gUnk_086149C4
#include "global.h"
typedef struct ActorControllerMotion {
    u8 pad0[0x50];
    s16 velocity[3];
    u8 pad56[0xc8 - 0x56];
    u16 animation;
    u8 padCA[0xb32 - 0xca];
    u16 flags;
    u8 padB34[5];
    u8 angle;
    u8 padB3A[8];
    u8 decay;
} ActorControllerMotion;
void sub_0818323C(ActorControllerMotion *);
extern const s16 gSineTable[];
static inline int trunc12(int x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
void sub_0818351C(ActorControllerMotion *p)
{
    sub_0818323C(p);
    {
        u8 *a = &p->angle;
        int shift = p->decay >> 1;
        int speed = 20;
        s16 *v = p->velocity;
        int angle = *a;
        speed >>= shift;
        {
            int x = trunc12(gSineTable[(angle + 64) & 255] * speed);
            int zero = 0;
            v[0] = x;
            v[1] = zero;
        }
        {
            const s16 *table = gSineTable;
            const s16 *entry = (const void *)((angle * 2) + (u32)table);
            v[2] = trunc12(*entry * speed);
        }
    }
    if (p->velocity[0] || p->velocity[2]) {
        u16 *flags = &p->flags;
        u32 value = *flags;
        u32 mask = 0x20;
        value |= mask;
        *flags = value;
        return;
    }
    p->flags &= ~0x20;
}
