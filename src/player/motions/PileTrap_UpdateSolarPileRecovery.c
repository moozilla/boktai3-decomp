#include "global.h"
#include "player/player.h"
#include "player/player_slot_status.h"
s32 Player_ConsumeStateEntry(void *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Player_SetActionPhase(void *, u8);
void Player_SetActionOrigin(void *);
void ActorSequence_Begin(void *, s32, s32);
s32 Player_CanRecoverResourceWhileFlagged(void *);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
void ConvergingEffects_EmitNearParticle(void *, void *);
void Player_OnRecoveryActionEnd(void *);
void Player_SetPendingCallback(void *, void (*)(void *));
s32 PileTrap_TryActivate(void *);
void PileTrap_BeginRecovery(void *);
// confidence=low: refreshes the barrier through the resource recovery phases.
void PileTrap_UpdateSolarPileRecovery(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        Player_SetPendingCallback(p, Player_OnRecoveryActionEnd);
    }
    switch (p->tetherPhase) {
        case 0:
            PileTrap_TryActivate(p->actionRequestEffect);
            Sound_PlaySong(0x128);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
            Player_SetActionPhase(p, 1);
            break;
        case 1:
            PileTrap_TryActivate(p->actionRequestEffect);
            if (p->tetherFrame == 0) {
                if ((u8)(p->facing - 2) <= 4) {
                    p->facing = 4;
                    p->heading = 1;
                    p->side = 0;
                } else {
                    p->facing = 0;
                    p->heading = 0;
                    p->side = 0;
                }
                Player_SetActionOrigin(p);
                ++p->tetherFrame;
            }
            p->resetValue6FC = 15;
            if (!Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 8), 64, 1)) {
                break;
            }
            Sound_PlaySong(0xcb);
            if (!(p->inputKeys[0] & 1)) {
                Player_SetActionPhase(p, 4);
                break;
            }
            if (Player_CanRecoverResourceWhileFlagged(p)) {
                p->chargeSoundActive = 1;
                p->chargeCounter = 0;
                Player_SetActionPhase(p, 2);
                break;
            }
            p->chargeCounter = 0;
            Player_SetActionPhase(p, 3);
            break;
        case 2:
            PileTrap_TryActivate(p->actionRequestEffect);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 9), 64, 1);
            p->resetValue6FC = 30;
            if (!(p->inputKeys[0] & 1))
                Player_SetActionPhase(p, 4);
            break;
        case 3:
            if (PileTrap_TryActivate(p->actionRequestEffect) == 1 && p->chargeCounter <= 19) {
                if (++p->chargeCounter > 19)
                    Sound_PlaySong(0xF5);
            }
            Player_SetStatusBits(&p->statusFlags, 0x80);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            p->resetValue6FC = 30;
            if (!(p->inputKeys[0] & 1)) {
                Player_SetActionPhase(p, 4);
                break;
            }
            if (!Player_CanRecoverResourceWhileFlagged(p)) {
                break;
            }
            Sound_PlaySong(0xd8);
            p->chargeSoundActive = 1;
            p->chargeCounter = 0;
            Player_SetActionPhase(p, 2);
            break;
        case 4:
            p->resetValue6FC = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 11), 64, 1)) {
                p->heading = p->facing;
                p->side = 0;
                ActorSequence_Begin(p, 0, 0);
                PileTrap_BeginRecovery(p->actionRequestEffect);
            }
            break;
    }
}
