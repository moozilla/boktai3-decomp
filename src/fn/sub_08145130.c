#define Player_UpdateCountdownAnimation sub_08145130
#include "global.h"
#include "player.h"
void Player_AddControlFlags(struct Player *, s32);
s32 sub_081419D4(u32, s32);
s32 sub_0813AE18(struct Player *, s32, s32, s32);
void Player_SetActionPhase(struct Player *, u8);
void sub_0813ADD0(struct Player *);
void sub_0813AFEC(struct Player *, s32, s32);
void Sound_PlaySong(u32);
void sub_08145130(struct Player *p)
{
    Player_AddControlFlags(p, 8);
    switch (p->tetherPhase) {
        case 0:
            if (p->animationIndex != 0x2C) {
                p->facing |= 1;
                if (p->facing > 4) {
                    p->heading = (8 - p->facing) >> 1;
                    p->side = 1;
                } else {
                    p->heading = p->facing >> 1;
                    p->side = 0;
                }
            }
            sub_0813AE18(p, sub_081419D4(p->actionMode, 18), 64, 1);
            if (p->actionCountdown <= 30) {
                p->actionCountdown = 0;
                Sound_PlaySong(0x1B0);
                Player_SetActionPhase(p, 1);
            } else
                p->actionCountdown -= 30;
            break;
        case 1:
            if (sub_0813AE18(p, sub_081419D4(p->actionMode, 19), 64, 1)) {
                sub_0813ADD0(p);
                sub_0813AFEC(p, 0, 0);
            }
            break;
    }
}
