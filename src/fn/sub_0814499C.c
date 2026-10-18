#define Player_UpdateActionEffectSequence sub_0814499C
#define Player_ConsumePhaseEntry Player_ConsumeStateEntry
#include "global.h"
#include "player.h"
u32 Player_ConsumeStateEntry(struct Player *);
void Player_SetActionPhase(struct Player *, u8);
s32 Player_CanSpendSharedResource(struct Player *, s32);
s32 sub_08162BE4(void *, struct SceneVec *);
s32 sub_0813AE18(struct Player *, s32, s32, s32);
void sub_0813AFEC(struct Player *, s32, s32);
void Sound_PlaySong(u32);
void sub_0814499C(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->tetherStatusFlags)
            p->actionEighteenEnabled = 0;
        else if (Player_CanSpendSharedResource(p, 3))
            p->actionEighteenEnabled = 1;
        else
            p->actionEighteenEnabled = 0;
        Player_SetActionPhase(p, 0);
    }
    switch (p->tetherPhase) {
        case 0:
            if (sub_0813AE18(p, 0x8B, 64, 1)) {
                if (!sub_08162BE4(p->actionRequestEffect, &p->position))
                    Sound_PlaySong(0x192);
                else {
                    Sound_PlaySong(0xD0);
                    Sound_PlaySong(0x3E6);
                }
                Player_SetActionPhase(p, 1);
            }
            break;
        case 1:
            if (++p->tetherFrame > 11)
                Player_SetActionPhase(p, 3);
            break;
        case 3:
            if (sub_0813AE18(p, 0x90, 64, 1))
                sub_0813AFEC(p, 0, 1);
            break;
    }
}
