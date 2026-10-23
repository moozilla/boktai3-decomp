#include "global.h"

typedef struct ActorAnimation {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[5];
    u8 finished, enter;
    u8 changed;
    u8 padB2;
    u8 locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    u8 (*tryUpdate)(void *);
    u8 (*reset)(void *);
    u8 pad10C[8];
    s8 savedAngle;
    u8 pad115[0x22e];
    u8 variant;
    s16 scale, minScale, maxScale, scaleStep;
    u8 pad34C[0x60];
    u32 mode;
} ActorAnimation;

u32 Callback_InvokeActor(void *, void *);
extern s16 gUnk_086149C4[];
static inline int trunc12(int x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
typedef struct {
    u32 a, b;
} Point;
// confidence=low: restores the saved position while applying a decreasing shake.
#define Ratatosk_UpdateDecayingShake sub_081F5E60
void Ratatosk_UpdateDecayingShake(ActorAnimation *p, int tick)
{
    ActorAnimation *p2 = p;
    u8 *position = (u8 *)p + 0x48;
    if (p->changed) {
        p->changed = 0;
        {
            u32 mask = 16;
            p->flags |= mask;
        }
        *(Point *)((u8 *)p + 0xd0) = *(Point *)(position + 12);
    }
    {
        u32 mask = 0x0200000C;
        p->flags |= mask;
    }
    if (!(u8)Callback_InvokeActor(p2, p->reset)) {
        *(Point *)(position + 12) = *(Point *)((u8 *)p + 0xd0);
        if (tick > 31)
            p->finished = 1;
        else {
            int dir = 3 & ~tick;
            int remaining = 32 - tick;
            s16 *velocity = (s16 *)((u8 *)p + 0x5c);
            u32 angle;
            int speed;
            angle = dir << 6;
            speed = remaining << 3;
            {
                int x = trunc12(gUnk_086149C4[(angle + 64) & 255] * speed);
                int zero = 0;
                velocity[0] = x;
                velocity[1] = zero;
            }
            {
                s16 *entry = &gUnk_086149C4[angle];
                velocity[2] = trunc12(*entry * speed);
            }
        }
    }
}
