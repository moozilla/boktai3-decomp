// MATCHING_CONCESSION: register-pinning
#include "global.h"
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
struct BurstA {
    s32 a;
    u32 b : 16;
    u32 c : 16;
};
struct BurstB {
    u32 a : 16;
    u32 b : 16;
    u32 c : 16;
    u32 d : 16;
};

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
void Player_ReleaseActionEffectOnExit(void *);
void Player_SetPendingCallback(void *, void (*)(void *));
void Player_UpdateAttachedEntryPosition(void *);
s32 Task4E01_GetKind7Value(s32);
static inline u32 bonus(u8 *p, u32 mask)
{
    u32 offset;
    u32 *field;
    u32 bits;
    if (!p)
        return 0;
    offset = 0x444;
    field = (u32 *)(p + offset);
    bits = mask;
    return *field & bits;
}
// confidence=low: attached task tiers and buffered restart define this charge handler.
#define Player_UpdateAttachedAttackCharge sub_0814F328
void Player_UpdateAttachedAttackCharge(u8 *p)
{
    struct BurstA ba;
    struct BurstB bb;
    u16 *t;
    u16 *r;
    if (Player_ConsumeStateEntry(p)) {
        u16 *first = (u16 *)(p + 0xC0A);
        s32 zero = 0;
        *first = zero;
        *(s32 *)(p + 0xC74) = --zero;
        Player_SetPendingCallback(p, Player_ReleaseActionEffectOnExit);
    }
    switch (BYTE(0x458)) {
        case 0:
            if (ANIMATE(0x26)) {
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
                ANIMATE(0x27);
            }
            if (++*t > 6) {
                BYTE(0xC0F) = 0;
                HALF(0x4D2) = 0;
                if (HALF(0x428) <= 2) {
                    Sound_PlaySong(0xD5);
                    goto release;
                }
                if (bonus(p, 0x10000000))
                    Player_SetActionPhase(p, 4);
                else
                    Player_SetActionPhase(p, 2);
                if (!(bonus(p, 0x10000000)))
                    Player_SetActionPhase(p, 4);
                else {
                    if (!Player_SpawnSelectedAttackProjectile(p))
                        goto release;
                    Player_SetActionPhase(p, 2);
                }
            }
            goto attached;
        case 2:
            if (++HALF(0x45A) == 6)
                Sound_PlaySong(0x1F4);
            if (!HELD)
                goto fire;
            if (Task4E01_GetKind7Value(*(s32 *)(p + 0xC74)) > 1) {
                BYTE(0xC0F) = 1;
                HALF(0x4D2) = 30;
                SetChargeParameter(p);
                HALF(0xAD8) = 36;
                Player_SetActionPhase(p, 3);
            }
            goto face;
        case 3: {
            register u32 inputOffset asm("r2") = 0x348;
            register u16 **input asm("r0");
            asm("" : "+r"(inputOffset));
            input = (u16 **)(p + inputOffset);
            if (!(**input & 2))
                goto fire;
            if (Task4E01_GetKind7Value(*(s32 *)(p + 0xC74)) > 2) {
                BYTE(0xC0F) = 1;
                HALF(0x4D2) = 30;
                SetChargeParameter(p);
                HALF(0xAD8) = 36;
                Sound_PlaySong(0x1F5);
                Player_SetActionPhase(p, 4);
            }
            goto face;
        }
        case 4: {
            register u32 inputOffset asm("r2") = 0x348;
            register u16 **input asm("r0");
            asm("" : "+r"(inputOffset));
            input = (u16 **)(p + inputOffset);
            if (!(**input & 2)) {
            fire:
                if (Player_SpawnSelectedAttackProjectile(p))
                    Sound_PlaySong(0x1F6);
            release:
                Player_SetActionPhase(p, 5);
            } else {
            face:
                Player_UpdateAttackFacing(p);
                ANIMATE(0x27);
            }
        attached:
            Player_UpdateAttachedEntryPosition(p);
            break;
        }
        case 5:
            ANIMATE(0x28);
            if (PRESSED)
                BYTE(0xBF6) = 1;
            r = (u16 *)(p + 0x4D2);
            if (*r) {
                s32 ang, n;
                if (*r > 5) {
                    u32 upper = 0xFFFF0000;
                    u32 lower = 0xFFFF;
                    asm("" : : "r"(upper), "r"(lower));
                    ba.a = 0;
                    ba.b = 0;
                    bb.a = 32;
                    bb.b = 32;
                    bb.c = 32;
                    SceneParticleBurst_CreateWhilePaused(p + 0x30, &ba, &bb, 1, 3, 4);
                }
                ang = ((BYTE(0x3A4) + 5) & 7) << 5;
                n = *r;
                *(s16 *)(p + 0x38) = -scale(gUnk_086149C4[(ang + 64) & 255] * n);
                *(s16 *)(p + 0x3C) = -scale(HALF(0x4D2) * gUnk_086149C4[ang]);
                HALF(0x4D2) = HALF(0x4D2) * 3 >> 2;
            } else if (++HALF(0x45A) > 4)
                Player_SetActionPhase(p, 6);
            break;
        case 6: {
            register u32 pending asm("r0");
            ANIMATE(0x27);
            if (PRESSED)
                BYTE(0xBF6) = 1;
            pending = BYTE(0xBF6);
            if (pending) {
                *(s32 *)(p + 0xC74) = -1;
                BYTE(0xBF6) = 0;
                BYTE(0x458) = 1;
            } else if (++HALF(0x45A) > 14) {
                BYTE(0xBF6) = 0;
                Player_SetActionPhase(p, 7);
            }
            break;
        }
        case 7:
            ANIMATE(0x26);
            if (++HALF(0x45A) > 4)
                ActorSequence_Begin(p, 0, 0);
            break;
    }
    BYTE(0xAC9) = 4;
}
