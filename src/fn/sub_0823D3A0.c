#include "global.h"
#include "player.h"

s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(struct Player *, s32, s32);
void Player_SetTetherMode(struct Player *, s32, s32);
s32 Player_IsUnflaggedBelowResourceLimit(struct Player *);
void Player_SetActionOrigin_0823D00C(struct Player *);
void Player_PlayLocalSong(s32, u32);
void Player_StopLocalSong(s32, u32);
s32 Player_IsUnflaggedBelowResourceLimit(struct Player *);
void Player_EmitConvergingParticle(struct Player *, void *);

void Player_UpdateConvergingChargeAction(struct Player *p)
{
    if (p->tetherMode != 7) {
        Player_SetTetherMode(p, 7, 0);
    }
    switch (p->tetherPhase) {
        case 0:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0), 0x40);
            Player_SetTetherMode(p, 7, 1);
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
                Player_SetActionOrigin_0823D00C(p);
                p->tetherFrame++;
            }
            p->resetValue6FC = 15;
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 8), 0x40)) {
                s32 r;
                Player_PlayLocalSong(p->slot, 0xCB);
                if (!(p->inputKeys[0] & 1)) {
                    Player_SetTetherMode(p, 7, 4);
                } else {
                    r = Player_IsUnflaggedBelowResourceLimit(p);
                    if (r) {
                        Player_PlayLocalSong(p->slot, 0x2E4);
                        p->chargeSoundActive = 1;
                        p->chargeCounter = 0;
                        Player_SetTetherMode(p, 7, 2);
                    } else {
                        p->chargeCounter = r;
                        Player_SetTetherMode(p, 7, 3);
                    }
                }
            }
            break;
        case 2: {
            u8 z;
            u32 mask = 0xA0;
            p->statusFlags = mask | p->statusFlags;
            if (p->chargeCounter <= 4) {
                p->chargeCounter++;
                if (p->chargeCounter == 5)
                    Player_PlayLocalSong(p->slot, 0x239);
            }
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 9), 0x40);
            p->resetValue6FC = 30;
            Player_SetActionOrigin_0823D00C(p);
            Player_EmitConvergingParticle(p, &p->actionOrigin);
            p->actionEighteenParameter = 0x16F;
            if (p->tetherFrame <= 0x1F)
                p->actionEighteenDuration = p->tetherFrame;
            else
                p->actionEighteenDuration = 0x40 - p->tetherFrame;
            z = 0;
            p->tetherFrame++;
            if (p->tetherFrame > 0x40)
                p->tetherFrame = 0;
            if (!(p->inputKeys[0] & 1)) {
                Player_StopLocalSong(p->slot, 0x239);
                p->chargeSoundActive = z;
                Player_SetTetherMode(p, 7, 4);
            } else if (!Player_IsUnflaggedBelowResourceLimit(p)) {
                Player_StopLocalSong(p->slot, 0x239);
                p->chargeSoundActive = z;
                Player_SetTetherMode(p, 7, 3);
            }
            break;
        }
        case 3: {
            u32 mask = 0x80;
            p->statusFlags = mask | p->statusFlags;
        }
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 10), 0x40);
            p->resetValue6FC = 30;
            if (!(p->inputKeys[0] & 1)) {
                Player_SetTetherMode(p, 7, 4);
            } else if (Player_IsUnflaggedBelowResourceLimit(p)) {
                Player_PlayLocalSong(p->slot, 0x239);
                p->chargeSoundActive = 1;
                p->chargeCounter = 0;
                Player_SetTetherMode(p, 7, 2);
            }
            break;
        case 4:
            p->resetValue6FC = 15;
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 11), 0x40)) {
                p->heading = p->facing;
                p->side = 0;
                Player_SetTetherMode(p, 0, 0);
            }
            break;
    }
}
