#define gSineTable gUnk_086149C4
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
} ActorControllerMotion;
void sub_0818323C(ActorControllerMotion *);
extern const s16 gSineTable[];
static inline int trunc12(int x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
void ActorController_UpdateMovementVelocity(ActorControllerMotion *p)
{
    sub_0818323C(p);
    if ((u16)(p->animation - 7) <= 4) {
        s16 *v;
        u8 *a;
        int angle, speed;
        p->flags |= 0x20;
        a = &p->angle;
        v = p->velocity;
        angle = *a;
        speed = 25;
        {
            int x = trunc12(gSineTable[(angle + 64) & 255] * speed);
            int zero = 0;
            v[0] = x;
            v[1] = zero;
        }
        v[2] = trunc12(gSineTable[angle] * speed);
    } else
        p->flags &= ~0x20;
}
