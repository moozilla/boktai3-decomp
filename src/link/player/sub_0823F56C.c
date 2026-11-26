#include "global.h"
#include "player/player.h"
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

void LinkPlayer_SetScaledDirectionalVelocity(void *, u32);
void Player_UpdateActionFacingSide(void *);
void DirectionalEffect_PlaceSprite(void *, u32, void *);
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
            if ((++(p->tetherFrame)) > 6) {
                p->statModifierContext.chargeTier = 0;
                p->contactMagnitude = 0;
                if ((p->currentStatB) <= 2) {
                    Player_PlayLocalSong(p->slot, 0xD5);
                    Player_SetTetherMode(p, 3, 5);
                } else if ((*p->inputKeys) & 2) {
                    Player_SetTetherMode(p, 3, 4);
                } else {
                    goto release;
                }
            }
            break;

        case 2:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x27), 0x40);
            if (p->tetherFrame == 6)
                Player_PlayLocalSong(p->slot, 0x1F4);
            if (!(*p->inputKeys & 2)) {
                Player_PlayLocalSong(p->slot, 0x1F6);
                Player_SetTetherMode(p, 3, 5);
            } else if (++p->tetherFrame > 59) {
                p->statModifierContext.chargeTier = 1;
                p->contactMagnitude = 30;
                ((struct Player *)p)->actionEighteenParameter = 0x16E;
                ((struct Player *)p)->actionEighteenDuration = 0x24;
                Player_PlayLocalSong(p->slot, 0x1F5);
                Player_SetTetherMode(p, 3, 4);
            }
            break;
        case 4:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x27), 0x40);
            if (!(*p->inputKeys & 2)) {
            release:
                Player_PlayLocalSong(p->slot, 0x1F6);
                Player_SetTetherMode(p, 3, 5);
            }
            break;
        case 5:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x28), 0x40);
            if ((p->inputKeys)[1] & 2)
                p->statModifierContext.bufferedAttack = 1;
            if (p->contactMagnitude) {
                s32 angle, speed;
                if (p->contactMagnitude > 5) {
                    struct AttackVec offset, size;
                    offset.x = 0;
                    offset.y = 0;
                    offset.z = 0;
                    size.x = 32;
                    size.y = 32;
                    size.z = 32;
                    SceneParticleBurst_CreateWhilePaused(&p->position, &offset, &size, 1, 3, 4);
                }
                angle = ((p->facing + 5) & 7) << 5;
                speed = p->contactMagnitude;
                p->velocityX = -scale(gUnk_086149C4[(angle + 64) & 255] * speed);
                {
                    s32 zSpeed = p->contactMagnitude;
                    p->velocityZ = -scale(gUnk_086149C4[angle] * zSpeed);
                }
                p->contactMagnitude = ((p->contactMagnitude) * 3) >> 2;
            } else if ((++(p->tetherFrame)) > 4) {
                Player_SetTetherMode(p, 3, 6);
            }
            break;
        case 6: {
            u32 pending;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x27), 0x40);
            if ((p->inputKeys)[1] & 2)
                p->statModifierContext.bufferedAttack = 1;
            pending = p->statModifierContext.bufferedAttack;
            if (pending) {
                p->statModifierContext.bufferedAttack = 0;
                p->tetherPhase = 1;
            } else if (++p->tetherFrame > 14) {
                p->statModifierContext.bufferedAttack = 0;
                Player_SetTetherMode(p, 3, 7);
            }
            break;
        }
        case 7:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x26), 0x40);
            if ((++(p->tetherFrame)) > 4) {
                Player_SetTetherMode(p, 0, 0);
            }
            break;
    }
    p->actionVariant = 4;
}
