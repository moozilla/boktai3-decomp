#include "global.h"

struct P {
    u8 filler[0x3A2];
    u8 b3A2;
    u8 b3A3;
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
void sub_08152174(void);
void sub_081543F0(void);
void sub_08155008(void);
void sub_081503EC(void);

void PlayerCue_Motion7Variant3(struct P *p, s32 dir, u32 c)
{
    if (dir >= 0)
        p->dir = dir;
    p->c = c;
    Player_EnterKindTwo(p);
    if (p->state == 3) {
        p->dir |= 1;
        if (p->dir > 4) {
            p->b3A2 = (8 - p->dir) >> 1;
            p->b3A3 = 1;
        } else {
            p->b3A2 = p->dir >> 1;
            p->b3A3 = 0;
        }
        ActorSequence_Begin(p, 7, 3);
        p->fn = sub_08152174;
    } else {
        Player_UpdateFacingSide(p);
        if (p->state == 4) {
            ActorSequence_Begin(p, 7, 3);
            p->fn = sub_081543F0;
        } else if (p->state == 5) {
            ActorSequence_Begin(p, 7, 3);
            p->fn = sub_08155008;
        } else {
            ActorSequence_Begin(p, 7, 4);
            p->fn = sub_081503EC;
        }
    }
}
