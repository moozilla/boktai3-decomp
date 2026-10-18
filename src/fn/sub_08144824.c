#define Player_UpdateActionEighteen sub_08144824
#include "global.h"
#include "player.h"
s32 Player_CanSpendSharedResource(struct Player *, s32);
void *SolarBomb_Create(struct Player *, struct SceneVec *, s32);
void sub_0813AFEC(struct Player *, s32, s32);
s32 sub_0813AE18(struct Player *, s32, s32, s32);
void Sound_PlaySong(u32);
void sub_08144824(struct Player *p)
{
    if (p->tetherMode != 18) {
        if (p->tetherStatusFlags)
            p->actionEighteenEnabled = 0;
        else if (Player_CanSpendSharedResource(p, 3)) {
            p->actionEighteenEnabled = 1;
            SolarBomb_Create(p, &p->position, 0);
        } else
            p->actionEighteenEnabled = 0;
        sub_0813AFEC(p, 18, 0);
    }
    switch (p->tetherPhase) {
        case 0:
            if (p->actionEighteenEnabled) {
                p->actionEighteenParameter = 0x170;
                p->actionEighteenDuration = 32;
            }
            if (sub_0813AE18(p, 0x8B, 64, 1)) {
                if (p->actionEighteenEnabled)
                    sub_0813AFEC(p, 18, 1);
                else
                    sub_0813AFEC(p, 18, 2);
            }
            break;
        case 1:
            if (p->actionEighteenEnabled) {
                p->actionEighteenParameter = 0x170;
                p->actionEighteenDuration = 32;
            }
            if (++p->tetherFrame > 11)
                sub_0813AFEC(p, 18, 3);
            break;
        case 2:
            if (p->tetherFrame == 0)
                Sound_PlaySong(0x192);
            sub_0813AE18(p, 0x95, 64, 1);
            if (++p->tetherFrame > 11)
                sub_0813AFEC(p, 18, 3);
            break;
        case 3:
            if (sub_0813AE18(p, 0x90, 64, 1))
                sub_0813AFEC(p, 0, 1);
            break;
    }
}
