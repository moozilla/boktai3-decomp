#include "global.h"

struct P {
    u8 filler[0x418];
    u8 state;
    u8 filler3[0x58C - 0x419];
    void *fn;
    u16 target[3];
    u8 filler4[0x5A4 - 0x596];
    u32 c;
};

void Player_EnterKindTwo(struct P *);
void ActorSequence_Begin(struct P *, s32, s32);
void PlayerCue_UpdateTargetDash(void);

u32 PlayerCue_Motion38To(struct P *p, u16 *pos, u32 c)
{
    u32 r;

    p->c = c;
    if (p->state == 2) {
        Player_EnterKindTwo(p);
        ActorSequence_Begin(p, 0x26, 0);
        p->fn = PlayerCue_UpdateTargetDash;
        p->target[0] = pos[0];
        p->target[1] = pos[1];
        p->target[2] = pos[2];
        r = 1;
    } else {
        r = 0;
    }
    return r;
}
