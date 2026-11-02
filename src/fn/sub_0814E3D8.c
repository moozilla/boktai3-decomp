#include "player.h"
#include "global.h"
#include "contact_actor.h"
// confidence=low: role inferred from callers and callback phases.
// confidence=low: charge tiers and buffered input inferred from phase transitions.
extern const s16 gUnk_086149C4[];
void SceneParticleBurst_CreateWhilePaused(void *, void *, void *, u32, u32, u32);
u32 Player_ConsumeStateEntry(void *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Player_SetActionPhase(void *, u8);
void Player_UpdateAttackFacing(void *);
void Sound_PlaySong(u32);
u8 *Player_GetSpecialAttackDescriptor(s32);
s32 ActorResponse_Apply(void *, s32);
s32 Player_SpawnSelectedAttackProjectile(void *);
void sub_081DAD94(s32, s32);
void ActorSequence_Begin(void *, s32, s32);
s32 Player_GetAttackEffectType(u32);
#define BYTE(o)    (*(u8 *)(p + (o)))
#define HALF(o)    (*(u16 *)(p + (o)))
#define HELD       (**(u16 **)(p + 0x348) & 2)
#define PRESSED    ((*(u16 **)(p + 0x348))[1] & 2)
#define ANIMATE(n) Player_SetAnimation(p, ActorVariant_GetTableValue(BYTE(0x418), n), 64, 1)

static inline int scale(int n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline void SetChargeParameter(u8 *p)
{
    u16 *target = (u16 *)(p + 0xAD6);
    u32 value = 0x16E;
    *target = value;
}
void Player_UpdateSelectedAttackCharge(u8 *p)
{
    struct ContactPackedVector origin;
    struct ContactPackedVector spread;
    u16 *t;
    u16 *r;
    if (Player_ConsumeStateEntry(p))
        HALF(0xC0A) = 0;
    switch (BYTE(0x458)) {
        case 0:
            if (ANIMATE(0x23)) {
                u16 *duration = (u16 *)(p + 0xC10);
                u32 zero = 0;
                *duration = 60;
                BYTE(0x53A) = zero;
                Player_SetActionPhase(p, 1);
            }
            break;
        case 1:
            t = (u16 *)(p + 0x45A);
            if (!*t)
                Sound_PlaySong(0x1F7);
            if (HELD) {
                Player_UpdateAttackFacing(p);
                ANIMATE(0x24);
            }
            if (++*t > 6) {
                s32 cost = Player_GetSpecialAttackDescriptor(BYTE(0xBF4))[2];
                cost = ActorResponse_Apply(p, cost);
                BYTE(0xC0F) = 0;
                HALF(0x4D2) = 0;
                if (HALF(0x428) < cost) {
                    Sound_PlaySong(0xD5);
                    goto release;
                }
                HALF(0xC12) = 15;
                if (!HELD)
                    goto release;
                if (BYTE(0xBF4) == 2)
                    goto release;
                if (BYTE(0xBF4) != 8) {
                    Player_SetActionPhase(p, 4);
                    break;
                }
                if (!Player_SpawnSelectedAttackProjectile(p))
                    goto release;
                Player_SetActionPhase(p, 2);
            }
            break;
        case 2:
            HALF(0xC12)++;
            t = (u16 *)(p + 0x45A);
            if (*t == 6)
                Sound_PlaySong(0x1F8);
            if (!HELD)
                goto release;
            sub_081DAD94(*(s8 *)(p + 0x2D), *(s32 *)(p + 0xC74));
            ANIMATE(0x24);
            if (++*t >= HALF(0xC10)) {
                BYTE(0xC0F) = 1;
                HALF(0x4D2) = 30;
                SetChargeParameter(p);
                HALF(0xAD8) = 36;
                Sound_PlaySong(0x1F9);
                Player_SetActionPhase(p, 4);
                break;
            }
            break;
        case 3:
            HALF(0xC12)++;
            if (!HELD)
                goto release;
            Player_UpdateAttackFacing(p);
            ANIMATE(0x24);
            if (++HALF(0x45A) >= HALF(0xC10)) {
                BYTE(0xC0F) = 2;
                HALF(0x4D2) = 60;
                SetChargeParameter(p);
                HALF(0xAD8) = 36;
                Sound_PlaySong(0x1F9);
                Player_SetActionPhase(p, 4);
                break;
            }
            break;
        case 4:
            HALF(0xC12)++;
            if (!HELD || BYTE(0xBF4) == 2) {
            release:
                Player_SetActionPhase(p, 5);
                break;
            }
            Player_UpdateAttackFacing(p);
            ANIMATE(0x24);
            break;
        case 5:
            if (!HALF(0x45A)) {
                if (!Player_SpawnSelectedAttackProjectile(p))
                    Player_SetActionPhase(p, 6);
                HALF(0xC0A)++;
            }
            ANIMATE(0x25);
            if (PRESSED)
                BYTE(0xBF6) = 1;
            r = (u16 *)(p + 0x4D2);
            if (*r) {
                s32 ang, n;
                if (*r > 5) {
                    origin.x = 0;
                    origin.y = 0;
                    origin.z = 0;
                    spread.x = 32;
                    spread.y = 32;
                    spread.z = 32;
                    SceneParticleBurst_CreateWhilePaused(p + 0x30, &origin, &spread, 1, 3, 4);
                }
                ang = ((BYTE(0x3A4) + 5) & 7) << 5;
                n = *r;
                *(s16 *)(p + 0x38) = -scale(gUnk_086149C4[(ang + 64) & 255] * n);
                *(s16 *)(p + 0x3C) = -scale(HALF(0x4D2) * gUnk_086149C4[ang]);
                HALF(0x4D2) = HALF(0x4D2) * 3 >> 2;
            } else {
                if (BYTE(0xBF4) == 2 && HALF(0xC0A) <= 4) {
                    if (++HALF(0x45A) > 8) {
                    charged:
                        Player_SetActionPhase(p, 4);
                    }
                } else if (++HALF(0x45A) > 4)
                    Player_SetActionPhase(p, 6);
            }
            break;
        case 6: {
            u32 pending;
            ANIMATE(0x24);
            if (PRESSED)
                BYTE(0xBF6) = 1;
            pending = BYTE(0xBF6);
            if (pending) {
                BYTE(0xBF6) = 0;
                BYTE(0x458) = 1;
            } else if (++((struct Player *)p)->tetherFrame > 14) {
                BYTE(0xBF6) = 0;
                Player_SetActionPhase(p, 7);
            }
            break;
        }
        case 7:
            ANIMATE(0x23);
            if (++HALF(0x45A) > 4)
                ActorSequence_Begin(p, 0, 0);
            break;
    }
    BYTE(0xAC9) = Player_GetAttackEffectType(BYTE(0xC18));
}
