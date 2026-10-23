#include "global.h"
#include "player.h"
// confidence=low: held charge followed by decaying reverse motion suggests a charged dash.
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
void SceneParticleBurst_CreateWhilePaused(void *, void *, void *, u32, u32, u32);
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void Player_UpdateChargedDashAttack(u8 *p)
{
    s32 k2 = 0;
    switch (*((u8 *)(p + 0x458))) {
        case 0:
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*((u8 *)(p + 0x418)), 0x26),
                                         0x40)) {
                Player_SetTetherMode(p, 3, 1);
            }
            break;

        case 1:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*((u8 *)(p + 0x418)), 0x27),
                                     0x40);
            if ((++(*((u16 *)(p + 0x45A)))) > 6) {
                *((u8 *)(p + 0xC0F)) = 0;
                *((u16 *)(p + 0x4D2)) = 0;
                if ((*((u16 *)(p + 0x428))) <= 2) {
                    Player_PlayLocalSong(*((u32 *)(p + 0x18)), 0xD5);
                    Player_SetTetherMode(p, 3, 5);
                } else if ((*(*((u16 **)(p + 0x348)))) & 2) {
                    Player_SetTetherMode(p, 3, 4);
                } else {
                    goto release;
                }
            }
            break;

        case 2:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 0x27), 0x40);
            if (H(0x45A) == 6)
                Player_PlayLocalSong(W(0x18), 0x1F4);
            if (!(**(u16 **)(p + 0x348) & 2)) {
                Player_PlayLocalSong(W(0x18), 0x1F6);
                Player_SetTetherMode(p, 3, 5);
            } else if (++H(0x45A) > 59) {
                B(0xC0F) = 1;
                H(0x4D2) = 30;
                ((struct Player *)p)->actionEighteenParameter = 0x16E;
                ((struct Player *)p)->actionEighteenDuration = 0x24;
                Player_PlayLocalSong(W(0x18), 0x1F5);
                Player_SetTetherMode(p, 3, 4);
            }
            break;
        case 4:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 0x27), 0x40);
            if (!(**(u16 **)(p + 0x348) & 2)) {
            release:
                Player_PlayLocalSong(W(0x18), 0x1F6);
                Player_SetTetherMode(p, 3, 5);
            }
            break;
        case 5:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 0x28), 0x40);
            if ((*(u16 **)(p + 0x348))[1] & 2)
                B(0xBF6) = 1;
            if (H(0x4D2)) {
                s32 angle, speed;
                if (H(0x4D2) > 5) {
                    struct AttackVec offset, size;
                    offset.x = 0;
                    offset.y = 0;
                    offset.z = 0;
                    size.x = 32;
                    size.y = 32;
                    size.z = 32;
                    SceneParticleBurst_CreateWhilePaused(p + 0x30, &offset, &size, 1, 3, 4);
                }
                angle = ((B(0x3A4) + 5) & 7) << 5;
                speed = H(0x4D2);
                H(0x38) = -scale(gUnk_086149C4[(angle + 64) & 255] * speed);
                {
                    s32 zSpeed = H(0x4D2);
                    H(0x3C) = -scale(gUnk_086149C4[angle] * zSpeed);
                }
                *((u16 *)(p + 0x4D2)) = ((*((u16 *)(p + 0x4D2))) * 3) >> 2;
            } else if ((++(*((u16 *)(p + 0x45A)))) > 4) {
                Player_SetTetherMode(p, 3, 6);
            }
            break;
        case 6: {
            u32 pending;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 0x27), 0x40);
            if ((*(u16 **)(p + 0x348))[1] & 2)
                B(0xBF6) = 1;
            pending = B(0xBF6);
            if (pending) {
                B(0xBF6) = 0;
                B(0x458) = 1;
            } else if (++H(0x45A) > 14) {
                *((u8 *)(p + 0xBF6)) = k2;
                Player_SetTetherMode(p, 3, 7);
            }
            break;
        }
        case 7:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*((u8 *)(p + 0x418)), 0x26),
                                     0x40);
            if ((++(*((u16 *)(p + 0x45A)))) > 4) {
                Player_SetTetherMode(p, 0, 0);
            }
            break;
    }
    B(0xAC9) = 4;
}
