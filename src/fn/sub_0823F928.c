// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"
// confidence=low: charge phases and orbiting attack collider identify a spinning attack.
#define B(o) (*(u8 *)(p + (o)))
#define H(o) (*(u16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
struct AttackVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
extern const s16 gUnk_086149C4[];
void Player_SetTetherMode(void *, u32, u32);
u32 ActorVariant_GetTableValue(u32, u32);
u32 Player_SetAnimationIndex(void *, u32, u32);
u32 Player_UpdateFacingFromInputHistory(void *);

void Player_PlayLocalSong(u32, u32);
void Player_StopLocalSong(u32, u32);
u32 Player_GetConfiguredActionPower(void *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);

void Player_SetDirectionalVelocity_08238300(void *, u32);
void sub_08238238(void *);
void DirectionalEffect_PlaceSprite(void *, u32, void *);
void SceneParticleBurst_CreateWithPause(void *, void *, void *, u32, u32, u32);
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void Player_UpdateChargedSpinAttack(u8 *p)
{
    if (B(0x457) != 3)
        Player_SetTetherMode(p, 3, 0);
    switch (B(0x458)) {
        case 0:
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 0x23), 0x40))
                Player_SetTetherMode(p, 3, 1);
            break;
        case 1:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 0x24), 0x40);
            if (++H(0x45A) > 6) {
                B(0xC0F) = 0;
                if (H(0x428) <= 9) {
                    Player_PlayLocalSong(W(0x18), 0xD5);
                    Player_SetTetherMode(p, 3, 5);
                } else if ((**(u16 **)(p + 0x348)) & 2)
                    Player_SetTetherMode(p, 3, 2);
                else {
                    Player_PlayLocalSong(W(0x18), 0x203);
                    Player_SetTetherMode(p, 3, 5);
                }
            }
            break;
        case 2:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 0x24), 0x40);
            if (H(0x45A) == 6)
                Player_PlayLocalSong(W(0x18), 0x1F8);
            if (!(**(u16 **)(p + 0x348) & 2)) {
                Player_PlayLocalSong(W(0x18), 0x203);
                Player_SetTetherMode(p, 3, 5);
            } else if (++H(0x45A) > 29) {
                B(0xC0F) = 1;
                ((struct Player *)p)->actionEighteenParameter = 0x16F;
                ((struct Player *)p)->actionEighteenDuration = 0x24;
                Player_PlayLocalSong(W(0x18), 0x1F9);
                Player_SetTetherMode(p, 3, 4);
            }
            break;
        case 4:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 0x24), 0x40);
            if (!(**(u16 **)(p + 0x348) & 2)) {
                u32 power;
                Player_PlayLocalSong(W(0x18), 0x2E9);
                ((struct Player *)p)->statModifierContext.contactTimer =
                    ((8 - ((struct Player *)p)->facing) & 7) << 5;
                power = Player_GetConfiguredActionPower(p);
                Collider_SetAttackProperties(p + 0xB8C, power, 0x20, 0x10, 2, 30);
                Player_PlayLocalSong(W(0x18), 0x202);
                Player_SetTetherMode(p, 3, 8);
            }
            break;
        case 5:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 0x25), 0x40);
            if ((*(u16 **)(p + 0x348))[1] & 2)
                B(0xBF6) = 1;
            if (++H(0x45A) > 4)
                Player_SetTetherMode(p, 3, 6);
            break;
        case 6: {
            register u32 pending asm("r0");
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 0x24), 0x40);
            if ((*(u16 **)(p + 0x348))[1] & 2)
                B(0xBF6) = 1;
            pending = B(0xBF6);
            if (pending) {
                B(0xBF6) = 0;
                B(0x458) = 1;
            } else if (++H(0x45A) > 14) {
                B(0xBF6) = 0;
                Player_SetTetherMode(p, 3, 7);
            }
            break;
        }
        case 7:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 0x23), 0x40);
            if (++H(0x45A) > 4)
                Player_SetTetherMode(p, 0, 0);
            break;
        case 8: {
            struct AttackVec offset, size, pos;
            s32 angle;
            B(0x4B9) = 0;
            if (Player_UpdateFacingFromInputHistory(p))
                Player_SetDirectionalVelocity_08238300(p, 6);
            if (!(H(0x45A) & 3)) {
                offset.x = 0;
                offset.y = 0;
                offset.z = 0;
                size.x = 32;
                size.y = 0;
                size.z = 32;
                SceneParticleBurst_CreateWithPause(p + 0x30, &offset, &size, 1, 2, 4);
            }
            ((struct Player *)p)->statModifierContext.contactTimer =
                (((struct Player *)p)->statModifierContext.contactTimer + 16) & 255;
            DirectionalEffect_PlaceSprite(p, ((struct Player *)p)->statModifierContext.contactTimer,
                                          p + 0x30);
            pos = *(struct AttackVec *)(p + 0x30);
            angle = (0x1A0 - ((struct Player *)p)->statModifierContext.contactTimer) & 255;
            pos.x += scale(170 * gUnk_086149C4[(angle + 64) & 255]);
            pos.y += 230;
            pos.z += scale(170 * gUnk_086149C4[angle]);
            Collider_SetPosition(p + 0xB8C, &pos, 0);
            B(0xBCE) = angle;
            Collider_Refresh(p + 0xB8C);
            ((struct Player *)p)->facing =
                ((0x10F - ((struct Player *)p)->statModifierContext.contactTimer) & 255) >> 5;
            sub_08238238(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 0x25), 0x40);
            if (!(H(0x45A) & 15))
                Player_PlayLocalSong(W(0x18), 0x204);
            if (++H(0x45A) >= (B(0xC0F) << 5)) {
                Player_StopLocalSong(W(0x18), 0x202);
                Player_SetTetherMode(p, 3, 6);
            }
            break;
        }
    }
    B(0xAC9) = 9;
}
