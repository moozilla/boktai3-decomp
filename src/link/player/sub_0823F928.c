#include "global.h"
#include "effects/particles/scene_particle_burst_api.h"
#include "player/player.h"
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
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void Player_UpdateChargedSpinAttack(struct Player *p)
{
    if ((p->tetherMode) != 3) {
        Player_SetTetherMode(p, 3, 0);
    }
    switch (p->tetherPhase) {
        case 0:
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x23),
                                         0x40)) {
                Player_SetTetherMode(p, 3, 1);
            }
            break;
        case 1:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40);
            if (++p->tetherFrame > 6) {
                p->statModifierContext.chargeTier = 0;
                if (p->currentStatB <= 9) {
                    Player_PlayLocalSong(p->slot, 0xD5);
                    Player_SetTetherMode(p, 3, 5);
                } else if ((*p->inputKeys) & 2) {
                    Player_SetTetherMode(p, 3, 2);
                } else {
                    Player_PlayLocalSong(p->slot, 0x203);
                    Player_SetTetherMode(p, 3, 5);
                }
            }
            break;
        case 2:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40);
            if (p->tetherFrame == 6)
                Player_PlayLocalSong(p->slot, 0x1F8);
            if (!(*p->inputKeys & 2)) {
                Player_PlayLocalSong(p->slot, 0x203);
                Player_SetTetherMode(p, 3, 5);
            } else if (++p->tetherFrame > 29) {
                p->statModifierContext.chargeTier = 1;
                ((struct Player *)p)->actionEighteenParameter = 0x16F;
                ((struct Player *)p)->actionEighteenDuration = 0x24;
                Player_PlayLocalSong(p->slot, 0x1F9);
                Player_SetTetherMode(p, 3, 4);
            }
            break;
        case 4:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40);
            if (!(*p->inputKeys & 2)) {
                u32 power;
                Player_PlayLocalSong(p->slot, 0x2E9);
                ((struct Player *)p)->statModifierContext.contactTimer =
                    ((8 - ((struct Player *)p)->facing) & 7) << 5;
                power = Player_GetConfiguredActionPower(p);
                Collider_SetAttackProperties(&p->statModifierContext.attackCollider, power, 0x20,
                                             0x10, 2, 30);
                Player_PlayLocalSong(p->slot, 0x202);
                Player_SetTetherMode(p, 3, 8);
            }
            break;
        case 5:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x25), 0x40);
            if ((p->inputKeys)[1] & 2) {
                p->statModifierContext.bufferedAttack = 1;
            }
            if ((++(p->tetherFrame)) > 4) {
                Player_SetTetherMode(p, 3, 6);
            }
            break;
        case 6: {
            u32 pending;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40);
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
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x23), 0x40);
            if ((++(p->tetherFrame)) > 4) {
                Player_SetTetherMode(p, 0, 0);
            }
            break;
        case 8: {
            struct ContactPosition offset, size;
            struct AttackVec pos;
            s32 angle;
            p->cameraInputProcessed = 0;
            if (Player_UpdateFacingFromInputHistory(p)) {
                Player_SetDirectionalVelocity_08238300(p, 6);
            }
            if (!((p->tetherFrame) & 3)) {
                offset.x = 0;
                offset.y = 0;
                offset.z = 0;
                size.x = 32;
                size.y = 0;
                size.z = 32;
                SceneParticleBurst_CreateWithPause(
                    (const struct ContactPosition *)&((struct Player *)p)->position, &offset, &size,
                    1, 2, 4);
            }
            ((struct Player *)p)->statModifierContext.contactTimer =
                (((struct Player *)p)->statModifierContext.contactTimer + 16) & 255;
            DirectionalEffect_PlaceSprite(p, ((struct Player *)p)->statModifierContext.contactTimer,
                                          &p->position);
            pos = *(struct AttackVec *)(&p->position);
            angle = (0x1A0 - ((struct Player *)p)->statModifierContext.contactTimer) & 255;
            pos.x += scale(170 * gUnk_086149C4[(angle + 64) & 255]);
            pos.y += 230;
            pos.z += scale(170 * gUnk_086149C4[angle]);
            Collider_SetPosition(&p->statModifierContext.attackCollider, &pos, 0);
            p->statModifierContext.attackCollider.direction = angle;
            Collider_Refresh(&p->statModifierContext.attackCollider);
            ((struct Player *)p)->facing =
                ((0x10F - ((struct Player *)p)->statModifierContext.contactTimer) & 255) >> 5;
            sub_08238238(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x25), 0x40);
            if (!(p->tetherFrame & 15))
                Player_PlayLocalSong(p->slot, 0x204);
            if (++p->tetherFrame >= (p->statModifierContext.chargeTier << 5)) {
                Player_StopLocalSong(p->slot, 0x202);
                Player_SetTetherMode(p, 3, 6);
            }
            break;
        }
    }
    p->actionVariant = 9;
}
