#define Player_LaunchDirectionalEffect DirectionalSpriteEffect_LaunchFromFacing
#define Player_ResetDefaultAction PlayerCue_EnterStandUpdate
#define Player_ConsumePendingActionTrigger PlayerCue_CompleteAction
#include "global.h"
#include "player.h"
void Sound_PlaySong(u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void DirectionalSpriteEffect_LaunchFromFacing(void *, u32, struct SceneVec *);
void Player_UpdateFourPhaseAction(struct Player *p)
{
    s32 speed;
    s32 result;
    if (p->actionMode == 1)
        speed = 0x30;
    else
        speed = 0x40;
    switch (p->tetherPhase) {
        case 0:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x15), speed, 1)) {
                ActorSequence_Begin(p, 3, 1);
                PlayerCue_CompleteAction(p);
            }
            break;
        case 1:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x16), 0x40, 1);
            break;
        case 2:
            if (!p->tetherFrame) {
                if (!p->actionMode)
                    Sound_PlaySong(0x38C);
                else
                    Sound_PlaySong(0x38B);
                p->tetherFrame++;
            }
            result = Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x17), speed, 1);
            if (p->actionMode == 1 && p->actionCueFlags == 2 && !p->actionCueCounter)
                DirectionalSpriteEffect_LaunchFromFacing(p, p->facing, &p->position);
            if (result)
                ActorSequence_Begin(p, 3, 3);
            break;
        case 3:
            if (++p->tetherFrame > 3) {
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
