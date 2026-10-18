#define Player_LaunchDirectionalEffect DirectionalSpriteEffect_LaunchFromFacing
#define Player_ResetDefaultAction PlayerCue_EnterStandUpdate
#define Player_ConsumePendingActionTrigger PlayerCue_CompleteAction
#define Player_UpdateFourPhaseAction       sub_081511E0
#include "global.h"
#include "player.h"
void Sound_PlaySong(u32);
s32 sub_081419D4(u32, s32);
s32 sub_0813AE18(void *, s32, s32, s32);
void sub_0813AFEC(void *, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void DirectionalSpriteEffect_LaunchFromFacing(void *, u32, struct SceneVec *);
void sub_081511E0(struct Player *p)
{
    s32 speed;
    s32 result;
    if (p->actionMode == 1)
        speed = 0x30;
    else
        speed = 0x40;
    switch (p->tetherPhase) {
        case 0:
            if (sub_0813AE18(p, sub_081419D4(p->actionMode, 0x15), speed, 1)) {
                sub_0813AFEC(p, 3, 1);
                PlayerCue_CompleteAction(p);
            }
            break;
        case 1:
            sub_0813AE18(p, sub_081419D4(p->actionMode, 0x16), 0x40, 1);
            break;
        case 2:
            if (!p->tetherFrame) {
                if (!p->actionMode)
                    Sound_PlaySong(0x38C);
                else
                    Sound_PlaySong(0x38B);
                p->tetherFrame++;
            }
            result = sub_0813AE18(p, sub_081419D4(p->actionMode, 0x17), speed, 1);
            if (p->actionMode == 1 && p->actionCueFlags == 2 && !p->actionCueCounter)
                DirectionalSpriteEffect_LaunchFromFacing(p, p->facing, &p->position);
            if (result)
                sub_0813AFEC(p, 3, 3);
            break;
        case 3:
            if (++p->tetherFrame > 3) {
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
