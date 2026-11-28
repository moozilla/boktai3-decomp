#include "global.h"
#include "player/player.h"
void Player_SetSecondaryActionFlags(struct Player *, u32);
void Camera_SnapToWithoutCompletion(void *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Player_PlayLocalSong(u32, u32);
u32 ActorVariant_GetTableValue(u32, u32);
void Player_SetAnimationIndex(struct Player *, u32, u32);
void Player_SetTetherMode(struct Player *, u32, u32);
// confidence=low: linked action camera, screen fade and timed animation phases.
void Player_UpdateLinkedScreenTransition(struct Player *p)
{
    struct SceneVec position;
    Player_SetSecondaryActionFlags(p, 4);
    switch (p->tetherPhase) {
        case 0:
            position = p->position;
            position.y += 150;
            Camera_SnapToWithoutCompletion(&position);
            if (p->cueVariant)
                ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 16);
            else
                ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 1);
            Player_PlayLocalSong(p->slot, 0x1AD);
            if (p->facing > 4) {
                p->heading = (8 - p->facing) >> 1;
                p->side = 1;
            } else {
                p->heading = p->facing >> 1;
                p->side = 0;
            }
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 64);
            Player_SetTetherMode(p, 0x1B, 1);
            p->terrainRecoveryCounter = 0;
            break;
        case 1:
            if (++p->tetherFrame > 5) {
                Player_SetTetherMode(p, 0x1B, 2);
                p->terrainRecoveryCounter = 0;
                p->secondaryPriority = 1;
                Player_PlayLocalSong(p->slot, 0x1AE);
            }
            break;
        case 2:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x12), 64);
            p->terrainRecoveryCounter -= 60;
            if (p->tetherFrame == 4) {
                if (p->actionMode == 6)
                    Player_PlayLocalSong(p->slot, 0x2ED);
                else
                    Player_PlayLocalSong(p->slot, 0x305);
            }
            if (++p->tetherFrame > 29)
                Player_SetTetherMode(p, 0x1B, 3);
            break;
        case 3:
            p->terrainRecoveryCounter = 0;
            Player_SetSecondaryActionFlags(p, 1);
            break;
    }
}
