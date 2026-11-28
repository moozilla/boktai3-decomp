#include "player/player.h"
#include "scene/collision/contact_actor.h"
#include "effects/bat_flock.h"

// confidence=low: holding input charges recoil and release advances the tether attack.

extern const s16 gUnk_086149C4[];
void SceneParticleBurst_CreateWhilePaused(void *, void *, void *, u32, u32, u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(struct Player *, s32, s32);
void Player_SetTetherMode(struct Player *, s32, s32);
void Player_PlayLocalSong(s32 a, u32 b);
s32 Player_UpdateFacingFromInputHistory(struct Player *p);
void Player_SpawnActionSpark(struct Player *p);

void Player_UpdateChargedTetherStrike(struct Player *p)
{
    struct ContactPackedVector ba;
    struct ContactPackedVector bb;

    switch (p->tetherPhase) {
        case 0:
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x23), 0x40))
                Player_SetTetherMode(p, 3, 1);
            break;
        case 1:
            if (p->tetherFrame == 0)
                Player_PlayLocalSong(p->slot, 0x1F7);
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40);
            p->tetherFrame++;
            if (p->tetherFrame <= 6)
                break;
            p->statModifierContext.chargeTier = 0;
            p->contactMagnitude = 0;
            if (p->currentStatB <= 0x18) {
                Player_PlayLocalSong(p->slot, 0xD5);
                Player_SetTetherMode(p, 3, 5);
                break;
            }
            p->statModifierContext.chargeSparkTimer = 0xF;
            if (p->inputKeys[0] & 2) {
                Player_SetTetherMode(p, 3, 2);
                break;
            }
            Player_PlayLocalSong(p->slot, 0xCD);
            Player_SetTetherMode(p, 3, 5);
            break;
        case 2:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40);
            p->statModifierContext.chargeSparkTimer++;
            if (p->tetherFrame == 6)
                Player_PlayLocalSong(p->slot, 0x1F8);
            if (!(p->inputKeys[0] & 2)) {
                Player_PlayLocalSong(p->slot, 0xCD);
                Player_SetTetherMode(p, 3, 5);
                break;
            }
            p->tetherFrame++;
            if (p->tetherFrame <= 0x3B)
                break;
            p->statModifierContext.chargeTier = 1;
            p->contactMagnitude = 0x1E;
            p->actionEighteenParameter = 0x16E;
            p->actionEighteenDuration = 0x24;
            Player_PlayLocalSong(p->slot, 0x1F9);
            Player_SetTetherMode(p, 3, 4);
            break;
        case 4:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40);
            p->statModifierContext.chargeSparkTimer++;
            if (p->inputKeys[0] & 2)
                break;
            Player_PlayLocalSong(p->slot, 0xCD);
            Player_SetTetherMode(p, 3, 5);
            break;
        case 5: {
            s32 ang, n;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x25), 0x40);
            if (p->inputKeys[1] & 2)
                p->statModifierContext.bufferedAttack = 1;
            if (p->contactMagnitude != 0) {
                if (p->contactMagnitude > 5) {
                    ba.x = 0;
                    ba.y = 0;
                    ba.z = 0;
                    bb.x = 32;
                    bb.y = 32;
                    bb.z = 32;
                    SceneParticleBurst_CreateWhilePaused(&p->position, &ba, &bb, 1, 3, 4);
                }
                ang = ((p->facing + 5) & 7) << 5;
                n = p->contactMagnitude;
                p->velocityX = -scale(gUnk_086149C4[(ang + 0x40) & 0xFF] * n);
                p->velocityZ = -scale(p->contactMagnitude * gUnk_086149C4[ang]);
                p->contactMagnitude = (p->contactMagnitude * 3) >> 2;
            } else {
                p->tetherFrame++;
                if (p->tetherFrame > 4)
                    Player_SetTetherMode(p, 3, 6);
            }
            break;
        }
        case 6:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40);
            if (p->inputKeys[1] & 2)
                p->statModifierContext.bufferedAttack = 1;
            if (p->statModifierContext.bufferedAttack) {
                p->statModifierContext.bufferedAttack = 0;
                p->tetherPhase = 1;
                break;
            }
            p->tetherFrame++;
            if (p->tetherFrame > 0xE) {
                p->statModifierContext.bufferedAttack = 0;
                Player_SetTetherMode(p, 3, 7);
            }
            break;
        case 7:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x23), 0x40);
            p->tetherFrame++;
            if (p->tetherFrame > 4)
                Player_SetTetherMode(p, 0, 0);
            break;
        default:
            break;
    }
    if (p->statModifierContext.chargeSparkTimer > 0xE) {
        if (p->actionMode == 1)
            Player_SpawnActionSpark(p);
        p->statModifierContext.chargeSparkTimer = 0;
    }
    p->actionVariant = 8;
}
