#define Player_UpdateBriefAction     sub_08146870
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
void sub_08146870(struct Player *p)
{
    if (Player_ConsumeStateEntry(p))
        Sound_PlaySong(0x57D);
    Player_AddControlFlags(p, 8);
    if (++p->tetherFrame > 15) {
        sub_0813AFEC(p, 0, 0);
    } else {
        Player_AddControlFlags(p, 2);
        if ((u16)(p->tetherFrame - 5) <= 7)
            Player_AddControlFlags(p, 1);
        Player_AddControlFlags(p, 4);
        p->actionEighteenParameter = 0x174;
        p->actionEighteenDuration = 0x18;
        if (!p->movementVelocityLocked)
            sub_0813AF18(p, 0x20);
        sub_0813ADD0(p);
        sub_0813AE18(p, sub_081419D4(p->actionMode, 1), 0x40, 1);
    }
}
