#include "global.h"

struct Pos {
    u32 x, y;
};

struct P {
    u8 filler[0x418];
    u8 state;
    u8 filler2[0x464 - 0x419];
    u8 b464;
    u8 b465;
    u8 filler3[0x58C - 0x466];
    void *fn;
    struct Pos target;
    u8 filler4[0x59D - 0x598];
    u8 b59D;
    u8 filler5[0x5A4 - 0x59E];
    u32 c;
};

void Player_EnterKindTwo(struct P *);
void ActorSequence_Begin(struct P *, s32, s32);
void PlayerCue_UpdateWalkTarget(void);
void PlayerCue_UpdateSideWalkTarget(void);
void Player_AdvanceScriptDestination(void);

void PlayerCue_WalkTo(struct P *p, struct Pos *pos, u32 c, s32 a)
{
    p->target = *pos;
    p->c = c;
    p->b59D = a;
    if (p->state == 3) {
        if ((s8)a >= 0)
            p->b59D = ((s8)a >> 1) * 2 + 1;
        p->fn = PlayerCue_UpdateWalkTarget;
    } else if (p->state == 4) {
        p->fn = PlayerCue_UpdateSideWalkTarget;
    } else if (p->state == 5) {
        p->b464 = 1;
        p->b465 = 1;
        p->fn = Player_AdvanceScriptDestination;
    } else {
        p->fn = Player_AdvanceScriptDestination;
    }
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 1, 0);
}
