// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define B(o)       (*(u8 *)(p + (o)))
#define H(o)       (*(u16 *)(p + (o)))
#define ANIMATE(n) Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), n), 64, 1)
extern u8 *gGameState;
extern const s16 gUnk_086149C4[];
u32 Player_ConsumeStateEntry(void *);
void Player_SetActionPhase(void *, u8);
void Player_StopActionSongOnExit(void *);
void Player_SetPendingCallback(void *, void (*)(void *));
u32 ActorVariant_GetTableValue(u32, u32);
u32 Player_SetAnimation(void *, u32, u32, u32);
u8 *Player_GetSpecialAttackDescriptor(s32);
s32 ActorResponse_Apply(void *, s32);
void Sound_StopSong(u32);
void Sound_PlaySong(u32);
void Player_UpdateAttackFacing(void *);
void DirectionalSpriteEffect_PlaceOppositeAngle(void *, u32, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
u32 Player_GetAttackAttributeFlags(u32);
void Player_UpdateFacingSide(void *);
void ActorSequence_Begin(void *, u32, u32);
u32 Player_GetAttackEffectType(u32);
void AnimData_SelectPaletteBank(void *, u32);
struct AttackVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
static inline s32 scale(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
// confidence=low: held input sustains the rotating attack collider while consuming energy.
#define Player_UpdateSustainedSpinAttack sub_0814EEE0
void Player_UpdateSustainedSpinAttack(u8 *p)
{
    struct AttackVec pos;
    if (Player_ConsumeStateEntry(p)) {
        Player_SetActionPhase(p, 0);
        Player_SetPendingCallback(p, Player_StopActionSongOnExit);
    }
    switch (B(0x458)) {
        case 0:
            if (ANIMATE(0x23)) {
                s32 amount;
                register s32 cost asm("r1");
                H(0xC10) = 30;
                Player_GetSpecialAttackDescriptor(B(0xBF4));
                if (B(0xC18) == 1)
                    amount = 0;
                else
                    amount = Player_GetSpecialAttackDescriptor(B(0xBF4))[2];
                cost = ActorResponse_Apply(p, amount);
                asm("" : "+r"(cost));
                if (H(0x428) < cost || B(0xC18) == 6) {
                    Player_SetActionPhase(p, 5);
                    Sound_StopSong(0xD5);
                } else {
                    Player_SetActionPhase(p, 1);
                    {
                        s32 angle = (B(0x3A4) << 5) & 255;
                        H(0xC0A) = angle;
                    }
                }
            }
            break;
        case 1:
            if (!H(0x45A))
                Sound_PlaySong(0x1F7);
            Player_UpdateAttackFacing(p);
            ANIMATE(0x24);
            if (++H(0x45A) > 6) {
                s32 enough;
                u8 *desc;
                s32 power;
                B(0xC0F) = 0;
                enough = 0;
                desc = Player_GetSpecialAttackDescriptor(B(0xBF4));
                if (!(H(0x45A) & 1)) {
                    s32 amount;
                    register s32 cost asm("r1");
                    if (B(0xC18) == 1)
                        amount = 0;
                    else
                        amount = Player_GetSpecialAttackDescriptor(B(0xBF4))[2];
                    cost = ActorResponse_Apply(p, amount);
                    if (H(0x428) < cost)
                        H(0x428) = 0;
                    else {
                        register s32 remaining asm("r0") = H(0x428) - cost;
                        H(0x428) = remaining;
                        enough = 1;
                    }
                } else
                    enough = 1;
                power = *(s16 *)(gGameState + 0x40) + desc[1];
                if (!(**(u16 **)(p + 0x348) & 2) || !enough) {
                    Sound_StopSong(0xCC);
                    Player_SetActionPhase(p, 6);
                } else {
                    s32 angle;
                    u8 *collider;
                    if (H(0x45A) == 8)
                        Sound_PlaySong(0xCC);
                    H(0xC0A) = (H(0xC0A) + 16) & 255;
                    DirectionalSpriteEffect_PlaceOppositeAngle(p, H(0xC0A), p + 0x30);
                    pos = *(struct AttackVec *)(p + 0x30);
                    angle = (416 - H(0xC0A)) & 255;
                    pos.x += scale(170 * gUnk_086149C4[(angle + 64) & 255]);
                    pos.y += 230;
                    pos.z += scale(170 * gUnk_086149C4[angle]);
                    collider = p + 0xB8C;
                    *(u16 *)(collider + 0x3C) = power;
                    Collider_SetPosition(collider, &pos, 0);
                    B(0xBCE) = angle;
                    Collider_Refresh(collider);
                    *(u32 *)(collider + 0x38) = Player_GetAttackAttributeFlags(B(0xC18));
                    {
                        s32 facing = ((H(0xC0A) + 15) & 255) >> 5;
                        B(0x3A4) = facing;
                    }
                    Player_UpdateFacingSide(p);
                }
                ANIMATE(0x25);
            }
            break;
        case 5:
            ANIMATE(0x25);
            if (++H(0x45A) > 4)
                Player_SetActionPhase(p, 6);
            break;
        case 6:
            ANIMATE(0x24);
            if (++H(0x45A) > 14) {
                Sound_StopSong(0xCC);
                B(0xBF6) = 0;
                Player_SetActionPhase(p, 7);
            }
            break;
        case 7:
            ANIMATE(0x23);
            if (++H(0x45A) > 4)
                ActorSequence_Begin(p, 0, 0);
            break;
    }
    B(0xAC9) = Player_GetAttackEffectType(B(0xC18));
    AnimData_SelectPaletteBank(p + 0x828, B(0xC18) + 75);
}
