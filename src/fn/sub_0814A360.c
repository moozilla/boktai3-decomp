#define Player_UpdateStatBEffectAction sub_0814A360
#define Player_StepActionAnimation     sub_0813AE18
#include "global.h"
#include "player.h"
void Sound_PlaySong(u32);
s32 sub_0813AE18(struct Player *, s32, s32, s32);
void sub_0813AFEC(struct Player *, s32, s32);
u32 Player_ConsumeStateEntry(struct Player *);
void Player_SetActionPhase(struct Player *, u8);
void *SolarBomb_Create(struct Player *, struct SceneVec *, s32);
void sub_0814A360(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->currentStatB > 1) {
            p->actionEighteenEnabled = 1;
            SolarBomb_Create(p, &p->position, 1);
            p->currentStatB -= 2;
        } else
            p->actionEighteenEnabled = 0;
    }
    switch (p->tetherPhase) {
        case 0:
            if (p->actionEighteenEnabled) {
                p->actionEighteenParameter = 0x170;
                p->actionEighteenDuration = 0x20;
            }
            if (sub_0813AE18(p, 0x8B, 0x40, 1)) {
                if (p->actionEighteenEnabled)
                    Player_SetActionPhase(p, 1);
                else
                    Player_SetActionPhase(p, 2);
            }
            break;
        case 1:
            if (p->actionEighteenEnabled) {
                p->actionEighteenParameter = 0x170;
                p->actionEighteenDuration = 0x20;
            }
            if (++p->tetherFrame > 11)
                Player_SetActionPhase(p, 3);
            break;
        case 2:
            if (!p->tetherFrame)
                Sound_PlaySong(0x192);
            sub_0813AE18(p, 0x95, 0x40, 1);
            if (++p->tetherFrame > 11)
                Player_SetActionPhase(p, 3);
            break;
        case 3:
            if (sub_0813AE18(p, 0x90, 0x40, 1))
                sub_0813AFEC(p, 0, 1);
            break;
    }
}
