#include "engine/shared/global.h"
#include "engine/shared/player.h"
#include "player/player_cue.h"
#include "engine/shared/engine_globals.h"

s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void Player_SetActionPhase(struct Player *p, u8 v);
void Player_AddControlFlags(struct Player *, s32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

void Player_UpdateDeathSequence(struct Player *p)
{
    Player_AddControlFlags(p, 4);
    switch (p->tetherPhase) {
        case 0: {
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 0x40, 1);
            Sound_PlaySong(0x1AD);
            p->secondaryPriority = 1;
            Player_SetActionPhase(p, 1);
            p->terrainRecoveryCounter = 0;
            p->followTarget = NULL;
            break;
        }
        case 1:
            p->tetherFrame++;
            if (p->tetherFrame > 0xe) {
                ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0x1FFF, 2);
                p->terrainRecoveryCounter = 0;
                Player_SetActionPhase(p, 2);
            }
            break;
        case 2:
            if (p->tetherFrame == 9) {
                if (p->actionMode == 6)
                    Sound_PlaySong(0x2EC);
                else
                    Sound_PlaySong(0x304);
            }
            p->tetherFrame++;
            if (p->tetherFrame > 0xe) {
                Player_SetActionPhase(p, 3);
                p->terrainRecoveryCounter = 0;
                Sound_PlaySong(0x1AF);
            }
            break;
        case 3:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x12), 0x40, 1);
            p->terrainRecoveryCounter -= 0x1E;
            p->tetherFrame++;
            if (p->tetherFrame > 0x27) {
                p->currentStatA = 0;
                Player_SetActionPhase(p, 4);
            } else {
                Sprite_BlinkUntilExpired(&PlayerCue_Graphics(p)->sprite, p->tetherFrame, 0xA, 0x14);
            }
            break;
        case 4:
            p->terrainRecoveryCounter = 0;
            Player_AddControlFlags(p, 1);
            p->tetherFrame++;
            if (p->tetherFrame > 0x3b) {
                Player_SetActionPhase(p, 5);
                gEngineFlags |= 0x400;
            }
            break;
        case 5:
            Player_AddControlFlags(p, 1);
            break;
    }
}
