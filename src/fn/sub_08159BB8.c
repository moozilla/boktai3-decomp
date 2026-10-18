#include "global.h"

struct P08159BB8 {
    u8 f0[0x20];
    u32 f20;
    u8 f24[0x236 - 0x24];
    u16 f236;
    u8 f238[0x270 - 0x238];
    u16 f270;
    u16 f272;
    u16 f274;
    u8 f276[0x3A2 - 0x276];
    u8 f3A2;
    u8 f3A3[0x464 - 0x3A3];
    u8 f464;
};
static inline void orr(u32 *a, u32 m)
{
    *a |= m;
}
void Player_EmitQueuedEffects(void *);
void ActorSequence_Begin(void *, u32, u32);

void Player_InitState25(struct P08159BB8 *p)
{
    orr(&p->f20, 0x1000);
    p->f464 = 0;
    p->f3A2 = 0;
    Player_EmitQueuedEffects(p);
    p->f236 |= 4;
    p->f270 = 0;
    p->f274 = 0;
    ActorSequence_Begin(p, 0x19, 0);
}
