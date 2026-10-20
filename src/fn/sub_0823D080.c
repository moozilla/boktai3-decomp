#include "global.h"
#include "player.h"

s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(struct Player *, s32, s32);
void Player_SetTetherMode(struct Player *, s32, s32);
s32 Player_IsFlaggedBelowResourceLimit(struct Player *);
void Player_SetActionOrigin_0823D00C(struct Player *);
void Player_PlayLocalSong(s32, u32);
void Player_StopLocalSong(s32, u32);
void sub_082392E8(struct Player *, void *);

void Player_UpdateChargeAction(struct Player *p)
{
    if (p->tetherMode != 7) {
        Player_SetTetherMode(p, 7, 0);
        p->actionVariant = 0;
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
                    r = Player_IsFlaggedBelowResourceLimit(p);
                    if (r) {
                        Player_PlayLocalSong(p->slot, 0xD8);
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
                if (p->chargeCounter == 5) {
                    if (p->actionMode == 1)
                        Player_PlayLocalSong(p->slot, 0x308);
                    else
                        Player_PlayLocalSong(p->slot, 0x128);
                }
            }
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 9), 0x40);
            z = 0;
            p->resetValue6FC = 30;
            Player_SetActionOrigin_0823D00C(p);
            sub_082392E8(p, &p->actionOrigin);
            if (!(p->inputKeys[0] & 1)) {
                Player_StopLocalSong(p->slot, 0xD8);
                p->chargeSoundActive = z;
                Player_SetTetherMode(p, 7, 4);
            } else if (!Player_IsFlaggedBelowResourceLimit(p)) {
                Player_StopLocalSong(p->slot, 0xD8);
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
            } else if (Player_IsFlaggedBelowResourceLimit(p)) {
                Player_PlayLocalSong(p->slot, 0xD8);
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
