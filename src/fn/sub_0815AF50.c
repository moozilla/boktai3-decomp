#include "global.h"

struct P {
    u8 filler[0x58C];
    void *fn;
    u8 filler2[0x59D - 0x590];
    u8 b;
    u8 filler3[0x5A4 - 0x59E];
    u32 c;
};

void Player_EnterKindTwo(struct P *);
void ActorSequence_Begin(struct P *, s32, s32);
void sub_0814FA10(void);

void PlayerCue_Face(struct P *p, u8 b, u32 c)
{
    p->b = b;
    p->c = c;
    p->fn = sub_0814FA10;
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 0, 0);
}
