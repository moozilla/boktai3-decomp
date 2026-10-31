#include "global.h"
#include "player.h"
// confidence=low: mode 27 camera lift and scale descent precede hiding.
void Player_AddControlFlags(void *, u32);
void Camera_SnapToWithoutCompletion(struct SceneVec *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Sound_PlaySong(u32);
void ActorSequence_Begin(void *, s32, s32);
void PlayerCue_CompleteAction(void *);
void PlayerCue_UpdateDeparture(struct Player *p)
{
    switch (p->tetherPhase) {
        case 0: {
            struct SceneVec pos;
            Player_AddControlFlags(p, 4);
            pos = p->position;
            pos.y += 150;
            Camera_SnapToWithoutCompletion(&pos);
            if (p->cueVariant <= 1) {
                if (p->cueVariant)
                    ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 16);
                else
                    ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 1);
            }
            if (p->facing > 4) {
                p->heading = (8 - p->facing) >> 1;
                p->side = 1;
            } else {
                p->heading = p->facing >> 1;
                p->side = 0;
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 64, 1);
            Sound_PlaySong(0x1AD);
            if (p->actionMode == 6)
                Sound_PlaySong(0x2ED);
            else
                Sound_PlaySong(0x305);
            p->terrainRecoveryCounter = 0;
            ActorSequence_Begin(p, 0x1B, 1);
            break;
        }
        case 1:
            Player_AddControlFlags(p, 4);
            if (++p->tetherFrame > 5) {
                p->terrainRecoveryCounter = 0;
                p->unkA2[4] = 1;
                Sound_PlaySong(0x1AE);
                ActorSequence_Begin(p, 0x1B, 2);
            }
            break;
        case 2:
            Player_AddControlFlags(p, 4);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x12), 64, 1);
            p->terrainRecoveryCounter -= 60;
            if (++p->tetherFrame > 29) {
                Player_AddControlFlags(p, 1);
                p->terrainRecoveryCounter = 0;
                ActorSequence_Begin(p, 0x1B, 3);
                PlayerCue_CompleteAction(p);
            }
            break;
        case 3:
            Player_AddControlFlags(p, 5);
            break;
    }
}
