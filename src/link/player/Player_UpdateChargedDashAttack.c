#include "global.h"
#include "scene/collision/contact_position.h"
#include "player/player.h"
// confidence=low: held charge followed by decaying reverse motion suggests a charged dash.
extern const s16 gUnk_086149C4[];
void Player_SetTetherMode(void *, u32, u32);
u32 ActorVariant_GetTableValue(u32, u32);
u32 Player_SetAnimationIndex(void *, u32, u32);
u32 Player_UpdateFacingFromInputHistory(void *);

void Player_PlayLocalSong(u32, u32);

void SceneParticleBurst_CreateWhilePaused(void *, void *, void *, u32, u32, u32);
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void Player_UpdateChargedDashAttack(struct Player *p)
{
    switch (p->tetherPhase) {
        case 0:
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x26),
                                         0x40)) {
                Player_SetTetherMode(p, 3, 1);
            }
            break;

        case 1:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x27), 0x40);
            if (++p->tetherFrame > 6) {
                p->statModifierContext.chargeTier = 0;
                p->contactMagnitude = 0;
                if (p->currentStatB <= 2) {
                    Player_PlayLocalSong(p->slot, 0xD5);
                    Player_SetTetherMode(p, 3, 5);
                } else if (!(p->inputKeys[0] & 2)) {
                    Player_PlayLocalSong(p->slot, 0x1F6);
                    Player_SetTetherMode(p, 3, 5);
                } else {
                    Player_SetTetherMode(p, 3, 4);
                }
            }
            break;

        case 2:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x27), 0x40);
            if (p->tetherFrame == 6)
                Player_PlayLocalSong(p->slot, 0x1F4);
            if (!(p->inputKeys[0] & 2)) {
                Player_PlayLocalSong(p->slot, 0x1F6);
                Player_SetTetherMode(p, 3, 5);
            } else if (++p->tetherFrame > 59) {
                p->statModifierContext.chargeTier = 1;
                p->contactMagnitude = 30;
                p->actionEighteenParameter = 0x16E;
                p->actionEighteenDuration = 0x24;
                Player_PlayLocalSong(p->slot, 0x1F5);
                Player_SetTetherMode(p, 3, 4);
            }
            break;
        case 4:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x27), 0x40);
            if (!(p->inputKeys[0] & 2)) {
                Player_PlayLocalSong(p->slot, 0x1F6);
                Player_SetTetherMode(p, 3, 5);
            }
            break;
        case 5:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x28), 0x40);
            if (p->inputKeys[1] & 2)
                p->statModifierContext.bufferedAttack = 1;
            if (p->contactMagnitude) {
                s32 angle;
                if (p->contactMagnitude > 5) {
                    struct ContactPosition offset, size;
                    offset.x = 0;
                    offset.y = 0;
                    offset.z = 0;
                    size.x = 32;
                    size.y = 32;
                    size.z = 32;
                    SceneParticleBurst_CreateWhilePaused(&p->position, &offset, &size, 1, 3, 4);
                }
                angle = ((p->facing + 5) & 7) << 5;
                p->velocityX = -scale(p->contactMagnitude * gUnk_086149C4[(angle + 64) & 255]);
                p->velocityZ = -scale(p->contactMagnitude * gUnk_086149C4[angle]);
                p->contactMagnitude = (p->contactMagnitude * 3) >> 2;
            } else if (++p->tetherFrame > 4)
                Player_SetTetherMode(p, 3, 6);
            break;
        case 6:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x27), 0x40);
            if (p->inputKeys[1] & 2)
                p->statModifierContext.bufferedAttack = 1;
            if (p->statModifierContext.bufferedAttack) {
                p->statModifierContext.bufferedAttack = 0;
                p->tetherPhase = 1;
            } else if (++p->tetherFrame > 14) {
                p->statModifierContext.bufferedAttack = 0;
                Player_SetTetherMode(p, 3, 7);
            }
            break;
        case 7:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x26), 0x40);
            if (++p->tetherFrame > 4)
                Player_SetTetherMode(p, 0, 0);
            break;
    }
    p->actionVariant = 4;
}
