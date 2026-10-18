#define Player_UpdateStatBAction     sub_0814A01C
#define Player_SelectActionAnimation sub_081419D4
#define Player_StepActionAnimation   sub_0813AE18
#include "global.h"
#include "player.h"
void Sound_PlaySong(u32);
u32 Player_ConsumeStateEntry(struct Player *);
void Player_AddControlFlags(struct Player *, s32);
void sub_0813AFEC(struct Player *, s32, s32);
void sub_0813AF18(struct Player *, s32);
void sub_0813ADD0(struct Player *);
s32 sub_081419D4(u32, s32);
s32 sub_0813AE18(struct Player *, s32, s32, s32);
void Player_SetActionPhase(struct Player *, u8);
void sub_0814A01C(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->currentStatB) {
            Sound_PlaySong(0x392);
            p->currentStatB--;
        } else {
            Sound_PlaySong(0x192);
            Player_SetActionPhase(p, 1);
        }
    }
    switch (p->tetherPhase) {
        case 0:
            if (++p->tetherFrame > 15) {
                sub_0813AFEC(p, 0, 0);
            } else {
                if (p->tetherFrame <= 7) {
                    p->actionEighteenParameter = 0x172;
                    p->actionEighteenDuration = 0x20;
                }
                if (!p->movementVelocityLocked)
                    sub_0813AF18(p, 0x20);
                sub_0813ADD0(p);
                sub_0813AE18(p, sub_081419D4(p->actionMode, 1), 0x40, 1);
            }
            break;
        case 1:
            sub_0813AE18(p, sub_081419D4(p->actionMode, 0), 0x40, 1);
            if (++p->tetherFrame > 7)
                sub_0813AFEC(p, 0, 0);
            break;
    }
}
