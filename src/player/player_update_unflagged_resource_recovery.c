#include "engine/shared/global.h"
#include "player/player.h"
#include "player/player_slot_status.h"
s32 Player_ConsumeStateEntry(void *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Player_SetActionPhase(void *, u8);
void Player_SetActionOrigin(void *);
void ActorSequence_Begin(void *, s32, s32);
s32 Player_CanRecoverResourceWhileUnflagged(void *);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
void ConvergingEffects_EmitFarParticle(void *, struct SceneVec *);
void Player_UpdateUnflaggedResourceRecovery(struct Player *p)
{
    s16 k2 = 0;
    if (Player_ConsumeStateEntry(p)) {
        Player_SetActionPhase(p, 0);
    }
    switch (p->tetherPhase) {
        case 0:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
            Player_SetActionPhase(p, 1);
            break;
        case 1:
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
            if (!(*p->inputKeys & 1)) {
                goto finish;
            }
            if (Player_CanRecoverResourceWhileUnflagged(p)) {
                Sound_PlaySong(0x2e4);
                p->chargeSoundActive = 1;
                p->chargeCounter = 0;
                Player_SetActionPhase(p, 2);
                break;
            }
            p->chargeCounter = 0;
            goto pause;
        case 2: {
            u32 zero;
            Player_SetStatusBits(&p->statusFlags, 0xa0);
            if (p->chargeCounter <= 4) {
                if (++p->chargeCounter == 5) {
                    Sound_PlaySong(0x239);
                }
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 9), 64, 1);
            p->resetValue6FC = 30;
            Player_SetActionOrigin(p);
            ConvergingEffects_EmitFarParticle(p, &p->actionOrigin);
            p->actionEighteenParameter = 0x16f;
            {
                u32 counter = p->tetherFrame;
                if (counter <= 31) {
                    p->actionEighteenDuration = counter;
                } else {
                    p->actionEighteenDuration = 64 - counter;
                }
            }
            {
                u16 *timer = &p->tetherFrame;
                u32 value = *timer + 1;
                zero = k2;
                *timer = value;
                if (((u16)value) > 64) {
                    *timer = 0;
                }
            }
            if (!(*p->inputKeys & 1)) {
                Sound_StopSong(0x239);
                p->chargeSoundActive = zero;
                goto finish;
            }
            if (Player_CanRecoverResourceWhileUnflagged(p)) {
                break;
            }
            Sound_StopSong(0x239);
            p->chargeSoundActive = zero;
        }
        pause:
            Player_SetActionPhase(p, 3);
            break;
        case 3:
            Player_SetStatusBits(&p->statusFlags, 0x80);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            p->resetValue6FC = 30;
            if (!(*p->inputKeys & 1)) {
            finish:
                Player_SetActionPhase(p, 4);
                break;
            }
            if (!Player_CanRecoverResourceWhileUnflagged(p)) {
                break;
            }
            Sound_PlaySong(0x239);
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
            }
            break;
    }
}
