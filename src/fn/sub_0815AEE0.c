#include "global.h"

struct P {
    u8 filler[0x94];
    u32 f;
    u8 filler2[0x58C - 0x98];
    void *fn;
};

void Player_EnterKindTwo(struct P *);
void ActorSequence_Begin(struct P *, s32, s32);
void PlayerCue_UpdateHidden(void);

void PlayerCue_Hide(struct P *p)
{
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 0, 0);
    p->f |= 1;
    p->fn = PlayerCue_UpdateHidden;
}
