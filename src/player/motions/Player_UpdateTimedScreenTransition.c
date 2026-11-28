#include "player/player.h"
#include "global.h"
#include "engine/engine.h"

// confidence=low: timed animation and screen-transition sequence; gameplay trigger is unknown.
void Player_AddControlFlags(void *, u32);
s32 ActorVariant_GetTableValue(u32, u32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Sound_PlaySong(u32);
void Player_SetActionPhase(void *, u8);
void sub_08232420(s32, s32, s32, s32, void *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Player_UpdateTimedScreenTransition(struct Player *p)
{
    Player_AddControlFlags(p, 4);
    switch (p->tetherPhase) {
        case 0:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 41), 64, 1);
            Sound_PlaySong(0x1AD);
            p->secondaryPriority = 1;
            Player_SetActionPhase(p, 1);
            p->terrainRecoveryCounter = 0;
            p->followTarget = NULL;
            break;
        case 1:
            if (++p->tetherFrame > 14) {
                Player_AddControlFlags(p, 1);
                Sound_PlaySong(0x1AE);
                Player_SetActionPhase(p, 2);
            }
            break;
        case 2:
            p->terrainRecoveryCounter -= 30;
            if (++p->tetherFrame > 3) {
                sub_08232420(1, 0, 0, 30, &p->position);
                Sound_PlaySong(0x145);
                p->currentStatA = 0;
                Player_SetActionPhase(p, 3);
            }
            break;
        case 3:
            Player_AddControlFlags(p, 1);
            if (++p->tetherFrame > 29) {
                ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0x1FFF, 2);
                Player_SetActionPhase(p, 4);
            }
            break;
        case 4:
            Player_AddControlFlags(p, 1);
            if (++p->tetherFrame > 29) {
                Player_SetActionPhase(p, 5);
                gEngineFlags |= 0x400;
            }
            break;
        case 5:
            Player_AddControlFlags(p, 1);
            break;
    }
}
