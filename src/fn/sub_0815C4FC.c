#include "global.h"

struct P {
    u8 filler[0x3A4];
    u8 dir;
    u8 filler2[0x418 - 0x3A5];
    u8 state;
    u8 filler3[0x58C - 0x419];
    void *fn;
    u8 filler4[0x5A4 - 0x590];
    u32 c;
};

void Player_EnterKindTwo(struct P *);
void ActorSequence_Begin(struct P *, s32, s32);
void Player_UpdateFacingSide(struct P *);
void sub_08153610(void);
void sub_08154CD4(void);
void sub_081556E0(void);

u32 PlayerCue_StateMotion(struct P *p, u32 c)
{
    p->c = c;
    if (p->state <= 1 || p->state == 6)
        return 0;
    if (p->state == 3) {
        Player_EnterKindTwo(p);
        ActorSequence_Begin(p, 0xD, 0);
        p->fn = sub_08153610;
    } else if (p->state == 4) {
        Player_EnterKindTwo(p);
        ActorSequence_Begin(p, 0xE, 0);
        p->fn = sub_08154CD4;
    } else if (p->state == 5) {
        p->dir = 4;
        Player_UpdateFacingSide(p);
        Player_EnterKindTwo(p);
        ActorSequence_Begin(p, 0xF, 0);
        p->fn = sub_081556E0;
    } else {
        return 0;
    }
    return 1;
}
