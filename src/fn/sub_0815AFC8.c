#include "global.h"

struct P {
    u8 filler[0x3A4];
    s8 b;
    u8 filler2[0x58C - 0x3A5];
    void *fn;
    u8 filler3[0x5A4 - 0x590];
    u32 c;
};

void Player_UpdateFacingSide(struct P *);
void Player_EnterKindTwo(struct P *);
void ActorSequence_Begin(struct P *, s32, s32);
void Player_UpdateTriggerAnimation(void);

void PlayerCue_StandVariant2(struct P *p, s32 v, u32 c)
{
    if (v >= 0)
        p->b = v;
    Player_UpdateFacingSide(p);
    p->c = c;
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 0, 2);
    p->fn = Player_UpdateTriggerAnimation;
}
