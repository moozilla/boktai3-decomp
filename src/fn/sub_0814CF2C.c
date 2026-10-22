#include "global.h"
#include "player.h"
void Sound_PlaySong(u32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_AddControlFlags(struct Player *, u32);

void sub_0814CF2C(struct Player *p)
{
    switch (p->tetherPhase) {
        case 0:
            Sound_PlaySong(0x39e);
            Player_SetAnimation(p, 0xe9, 0x40, 1);
            ActorSequence_Begin(p, 0x24, 1);
            break;
        case 1:
            break;
        case 2:
            if ((p->tetherFrame & 7) >= 7 - (p->tetherFrame >> 3))
                Player_AddControlFlags(p, 5);
            if (++p->tetherFrame > 0x37)
                ActorSequence_Begin(p, 0x24, 3);
            break;
        case 3:
            Player_AddControlFlags(p, 5);
            break;
    }
}
