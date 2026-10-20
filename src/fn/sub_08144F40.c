#include "global.h"
#include "player.h"
// confidence=low: departure variant with a camera lift and scale descent, sibling of PlayerCue_UpdateDeparture.
void Player_AddControlFlags(void *, u32);
void Camera_SnapToWithoutCompletion(struct SceneVec *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Sound_PlaySong(u32);
void ActorSequence_Begin(void *, s32, s32);
void sub_08144F40(struct Player *p)
{
    s32 phase;
    Player_AddControlFlags(p, 4);
    phase = p->tetherPhase;
    switch (phase) {
        case 0: {
            struct SceneVec pos;
            pos = p->position;
            pos.y += 150;
            Camera_SnapToWithoutCompletion(&pos);
            if (p->cueVariant)
                ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 16);
            else
                ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 1);
            Sound_PlaySong(0x1AD);
            if (p->facing > 4) {
                p->heading = (8 - p->facing) >> 1;
                p->side = 1;
            } else {
                p->heading = p->facing >> 1;
                p->side = 0;
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 64, 1);
            ActorSequence_Begin(p, 0x1B, 1);
            p->terrainRecoveryCounter = 0;
            break;
        }
        case 1:
            if (++p->tetherFrame > 5) {
                ActorSequence_Begin(p, 0x1B, 2);
                p->terrainRecoveryCounter = 0;
                *((u8 *)p + 0x153) = phase;
                Sound_PlaySong(0x1AE);
            }
            break;
        case 2:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x12), 64, 1);
            p->terrainRecoveryCounter -= 60;
            if (p->tetherFrame == 4) {
                if (p->actionMode == 6)
                    Sound_PlaySong(0x2ED);
                else
                    Sound_PlaySong(0x305);
            }
            if (++p->tetherFrame > 29)
                ActorSequence_Begin(p, 0x1B, 3);
            break;
        case 3:
            p->terrainRecoveryCounter = 0;
            Player_AddControlFlags(p, 1);
            break;
    }
}
